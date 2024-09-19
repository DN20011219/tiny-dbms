// Copyright (c) 2024 by dingning
//
// file  : connector.h
// since : 2024-08-09
// desc  : this class is used to create new worker thread.

#ifndef VDBMS_BENCH_CONNECTOR_H_
#define VDBMS_BENCH_CONNECTOR_H_

#include <string>
#include <vector>
#include <map>
#include <thread>
#include <condition_variable>
#include <iostream>
#include <functional>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "../config.h"

// 在mac平台上使用消息通道进行通信
#if defined(PLATFORM_IS_MAC)
    #include <sys/ipc.h>
    #include <sys/msg.h>
#endif

// 在win平台上使用命名管道实现相同的逻辑，从而进行通信
#if defined(PLATFORM_IS_WIN)
    #include <windows.h>

#endif

#include "./queue_msg.h"
#include "./worker.h"
#include "./server.h"

#include "../sql/executer/operator.h"

using std::vector;
using std::string;
using std::map;
using std::cout;
using std::cin;
using std::endl;


namespace tiny_v_dbms {

/**
 * 管道名称池管理类，通过名称资源池化的方式控制同时最多存在的连接数。目前只用在win版本上
 * 提供两个函数:
 * 1、Get函数将从池中取得一个未使用名称并标记为已使用
 * 2、Release函数将释放一个名称标记为未使用
 * 目前并不会遇到并发问题，原因在于连接的建立和释放都是由单线程控制的，加锁主要目的是为后续serve自动检测僵尸连接并自动销毁做准备
 */
class PipeNamePool {
private:
    std::vector<std::string> pipe_names;   // 管道名称池
    std::vector<bool> in_use;              // 管道名称使用标记
    std::mutex pool_mutex;                 // 线程安全的互斥锁

public:

    /**
     * 初始化名称资源池
     */
    PipeNamePool() {
        for (int i = 0; i < MAX_PIPE_CONNECTIONS; ++i)
        {
            pipe_names.push_back(PIPE_NAME_TEXT(WORKER_PIPE_NAME_SUFFIX) + std::to_string(i));
            in_use.push_back(false);
        }
    }

    /**
     * 获取一个未使用的管道名称
     * @todo: 调用方需要catch error并进行处理，以防止程序因此终止
     */
    std::string Get() {
        std::lock_guard<std::mutex> lock(pool_mutex);  // 自动管理互斥锁，防止竞态条件
        for (size_t i = 0; i < pipe_names.size(); ++i) 
        {
            if (!in_use[i]) {
                in_use[i] = true;  // 标记为使用中
                return pipe_names[i];
            }
        }
        throw std::runtime_error("No available pipe names, maximum connections reached.");
    }

    /**
     * 释放一个管道名称
     */
    void Release(const std::string& pipeName) {
        std::lock_guard<std::mutex> lock(pool_mutex);
        for (size_t i = 0; i < pipe_names.size(); ++i) 
        {
            if (pipe_names[i] == pipeName) {
                in_use[i] = false;  // 标记为未使用
                return;
            }
        }
        throw std::runtime_error("Pipe name not found in the pool.");
    }
};

class Connector
{
    friend class Server;

private:
    // store sessions
    vector<Session*> sessions;
    map<string, Session*> session_map;

    // cache the sessions - worker map from Server, Create Worker and worker thread when pruduce one new connection
    map<Session*, Worker*> worker_map;

    // thread control sign
    bool working;

    // used to cache connection information and result
#if defined(PLATFORM_IS_MAC)
    long msg_queue_id;  // this id will store the queue used to communicate
#endif
#if defined(PLATFORM_IS_WIN)
    std::string pipe_name;  // 使用pipename来指定管道
#endif
    string connect_ip; int connect_port; UserIdentity connect_identity; string connect_db_name; 
    Session* connect_result;

    // here use op to open db
    Operator* db_operator;

    // 管道名称池
    PipeNamePool* pipe_name_pool;
public:

    Connector(map<Session*, Worker*>& workers, Operator* op)
    {
        working = true;
        worker_map = workers;
        db_operator = op;

        pipe_name_pool = new PipeNamePool();
    }

    ~Connector()
    {
        // stop all worker threads
        ShutDown();

        delete pipe_name_pool;
    }

    Session* Connect(string ip, int port, UserIdentity identity, string db_name)
    {
        Session* new_session;

        // check session exist
        if (GetSession(ip, port, new_session))
        {
            // if areadly exist and identity is same as input, return connect true, and set old connnect information as back
            if (identity == new_session->connector_identity)
            {
                new_session->connect_state = true;
                return new_session;
            }

            // one ip and port can only connect one db as one role
            new_session->connect_state = false;
            return new_session;
        }

        if (db_name == DEFAULT_DB_FOLDER_NAME)
        {
            // create one unique worker thread which can send db_msg to db_worker_thread
            CreateNewRootSession(new_session, ip, port);
        }
        else
        {
            // create user worker and session
            CreateNewSession(new_session, ip, port, identity, db_name);
        }
        return new_session;
    } 


#if defined(PLATFORM_IS_MAC)
    // this thread will handle all create db and drop db sql, by communicate with user's work thread in special msg queue
    void RunBaseDBThread()
    {
        Session* new_session;
        new_session = new Session();
        new_session->msg_queue_id = BASE_DB_WORKER_RECEIVE_QUEUE_ID;
        new_session->client_ip = "0.0.0.0";
        new_session->client_port = 0;
        new_session->connect_db_name = DEFAULT_DB_FOLDER_NAME;
        new_session->connector_identity = ROOT;

        new_session->cached_db = new DB();
        new_session->cached_db->db_name = DEFAULT_DB_FOLDER_NAME;
        db_operator->OpenDB(new_session->cached_db, new_session->cached_db);

        sessions.push_back(new_session);
        session_map[DEFAULT_DB_FOLDER_NAME] = new_session;

        std::thread new_worker_thread([this, new_session]{
            this->worker_map[new_session] = new Worker(new_session, this->db_operator, new_session->cached_db);
            cout << "DB Worker: " << new_session->cached_db->db_name << " run successfully on: " << std::this_thread::get_id() << std::endl;
            this->worker_map[new_session]->BaseDBListenThread();
            });
        new_worker_thread.detach();
    }

        
    // this thread will watch the msg queue and serialize the information about connect 
    void RunForwardThread()
    {
        // check msg queue is ready
        if (connector_msg_key < 0)
        {
            throw std::runtime_error("msg queue not start successfully");
        }

        ConnectMsg msg;
        msg.msg_type = CONNECTOR_MESSAGE_ID;
        while(working){
            // get one call msg from public queue, contains one unique queue id used to communicate.
            // first msg data struct: | receive queue id | send back queue id |
            msgrcv(connector_msg_key, &msg, MSG_DATA_LENGTH, CONNECTOR_MSG_TYPE_RECV, 0);
            if (!working) break;

            // get receive queue id and send back queue id
            // memcpy(&msg.msg_type, msg.msg_data, sizeof(long));
            long special_queue_id;
            memcpy(&special_queue_id, msg.msg_data, sizeof(long));
            
            // get one detail message from special message queue(client send information msg to connect server)
            msgrcv(connector_msg_key, &msg, MSG_DATA_LENGTH, special_queue_id, 0);
            if (!working) break;
            
            // get connection information from input stream
            msg_queue_id = msg.msg_type;   // cache the special queue id, and it will be used lately between client and the sql worker of the client
                
            default_address_type offset = 0;

            char* ip_cache = new char[IP_LENGTH];
            memcpy(ip_cache, msg.msg_data + offset, IP_LENGTH);
            connect_ip = ip_cache;
            delete[] ip_cache;
            offset += IP_LENGTH;

            memcpy(&connect_port, msg.msg_data + offset, PORT_LENGTH);
            offset += PORT_LENGTH;

            int connect_identity_type;
            memcpy(&connect_identity_type, msg.msg_data + offset, IDENTITY_LENGTH);
            offset += IDENTITY_LENGTH;
            connect_identity = UserIdentity(connect_identity_type);

            int db_name_length;
            memcpy(&db_name_length, msg.msg_data + offset, CONNECT_DB_NAME_LENGTH_LENGTH);
            offset += CONNECT_DB_NAME_LENGTH_LENGTH;
            assert(db_name_length < CONNECT_DB_NAME_LENGTH);

            char* db_name_cache = new char[db_name_length];
            memcpy(db_name_cache, msg.msg_data + offset, db_name_length);
            connect_db_name = db_name_cache;
            delete[] db_name_cache;

            connect_result = Connect(connect_ip, connect_port, connect_identity, connect_db_name);
            ConnectMsg::SerializeConnectResult(connect_result, msg);

            msg.msg_type = special_queue_id + 1; // send back use send_back_queue_id + 1, because this queue has been used to receive information msg sent by client
            msgsnd(connector_msg_key, &msg, MSG_DATA_LENGTH, 0);
                
            connect_result = nullptr;
        }   
    }
#endif

#if defined(PLATFORM_IS_WIN)
    // 启动处理base db相关请求的工作线程，全部base db相关的请求都将被定向至该线程进行处理
    void RunBaseDBThread() {
        Session* new_session = new Session();
        new_session->pipe_name = SplicePipeName(BASE_DB_WORKER_REC_PIPE_NAME);
        new_session->client_ip = "0.0.0.0";
        new_session->client_port = 0;
        new_session->connect_db_name = DEFAULT_DB_FOLDER_NAME;
        new_session->connector_identity = ROOT;

        new_session->cached_db = new DB();
        new_session->cached_db->db_name = DEFAULT_DB_FOLDER_NAME;
        db_operator->OpenDB(new_session->cached_db, new_session->cached_db);

        sessions.push_back(new_session);
        session_map[DEFAULT_DB_FOLDER_NAME] = new_session;

        // 创建工作线程并启动
        std::thread new_worker_thread([this, new_session] {
            this->worker_map[new_session] = new Worker(new_session, this->db_operator, new_session->cached_db);
            std::cout << "DB Worker: " << new_session->cached_db->db_name 
                      << " run successfully on thread: " << std::this_thread::get_id() << std::endl;
            this->worker_map[new_session]->BaseDBListenThread();
        });
        new_worker_thread.detach();
    }

    /**
     * 连接建立线程将监听公共管道，获取建立连接的客户端信息后有以下任务需要完成：
     * 1、分配的工作通道名
     * 2、打开连接的数据库
     * 3、建立对应的工作线程
     * 4、储存对应的session
     * 5、发送给客户端连接建立结果，如果成功返回工作通道名，失败则返回错误提示信息
     */
    void RunForwardThread() {
        HANDLE hPipe;
        char buffer[MSG_DATA_LENGTH];
        DWORD bytesRead;

        bool working = true;

        while (working) {       
            
            // 每次循环都重新打开Pipe来获取单条消息
            hPipe = CreateNamedPipe(
                PIPE_NAME_TEXT(CONNECTOR_RECEIVE_PIPE_NAME), // 拼接pipe name
                PIPE_ACCESS_DUPLEX,       // 需要读写权限，读取客户端信息，向客户端发送分配的管道名
                PIPE_TYPE_MESSAGE |       // message-type pipe
                PIPE_READMODE_MESSAGE |   // message-read mode
                PIPE_WAIT,                // blocking mode
                1,                        // max. instances
                MSG_DATA_LENGTH,          // output buffer size
                MSG_DATA_LENGTH,          // input buffer size
                10,                       // client time-out，这里设置10
                NULL);                    // default security attribute

            if (hPipe == INVALID_HANDLE_VALUE) {
                throw std::runtime_error("Named pipe creation failed");
            }

            // Wait for the client to connect
            bool connected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

            if (!connected) {
                CloseHandle(hPipe);
                throw std::runtime_error("Pipe connection failed");
            }

            std::cout << "Pipe connected, waiting for messages..." << std::endl;

            if (ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL)) {

                std::cout << "Received message: " << buffer << std::endl;
                
                ConnectionRequest request;
                request.Deserialize(buffer);

                // open db
            } else {
                std::cerr << "Failed to read from pipe." << std::endl;
                break;
            }

            string pipe_name;
            try
            {
                pipe_name = pipe_name_pool->Get();
            }
            catch(std::runtime_error e)
            {
                DWORD bytesWritten;

                // 发送连接失败消息
                ConnectionResult result;
                result.state = false;
                result.inform = "No enough pipe resource";
                char* buffer = new char[result.Size()];
                result.Serialize(buffer);
                
                WriteFile(hPipe, buffer, result.Size(), &bytesWritten, NULL);

                delete[] buffer;
                CloseHandle(hPipe);

                continue;
            }

            //  todo:发送分配的pipe_name给客户端


            CloseHandle(hPipe);
        }


    }
    // this thread will watch the msg queue and serialize the information about connect 
    void RunForwardThread()
    {
        // check msg queue is ready
        if (connector_msg_key < 0)
        {
            throw std::runtime_error("msg queue not start successfully");
        }

        ConnectMsg msg;
        msg.msg_type = CONNECTOR_MESSAGE_ID;
        while(working){
            // get one call msg from public queue, contains one unique queue id used to communicate.
            // first msg data struct: | receive queue id | send back queue id |
            msgrcv(connector_msg_key, &msg, MSG_DATA_LENGTH, CONNECTOR_MSG_TYPE_RECV, 0);
            if (!working) break;

            // get receive queue id and send back queue id
            // memcpy(&msg.msg_type, msg.msg_data, sizeof(long));
            long special_queue_id;
            memcpy(&special_queue_id, msg.msg_data, sizeof(long));
            
            // get one detail message from special message queue(client send information msg to connect server)
            msgrcv(connector_msg_key, &msg, MSG_DATA_LENGTH, special_queue_id, 0);
            if (!working) break;
            
            // get connection information from input stream
            msg_queue_id = msg.msg_type;   // cache the special queue id, and it will be used lately between client and the sql worker of the client
                
            default_address_type offset = 0;

            char* ip_cache = new char[IP_LENGTH];
            memcpy(ip_cache, msg.msg_data + offset, IP_LENGTH);
            connect_ip = ip_cache;
            delete[] ip_cache;
            offset += IP_LENGTH;

            memcpy(&connect_port, msg.msg_data + offset, PORT_LENGTH);
            offset += PORT_LENGTH;

            int connect_identity_type;
            memcpy(&connect_identity_type, msg.msg_data + offset, IDENTITY_LENGTH);
            offset += IDENTITY_LENGTH;
            connect_identity = UserIdentity(connect_identity_type);

            int db_name_length;
            memcpy(&db_name_length, msg.msg_data + offset, CONNECT_DB_NAME_LENGTH_LENGTH);
            offset += CONNECT_DB_NAME_LENGTH_LENGTH;
            assert(db_name_length < CONNECT_DB_NAME_LENGTH);

            char* db_name_cache = new char[db_name_length];
            memcpy(db_name_cache, msg.msg_data + offset, db_name_length);
            connect_db_name = db_name_cache;
            delete[] db_name_cache;

            connect_result = Connect(connect_ip, connect_port, connect_identity, connect_db_name);
            ConnectMsg::SerializeConnectResult(connect_result, msg);

            msg.msg_type = special_queue_id + 1; // send back use send_back_queue_id + 1, because this queue has been used to receive information msg sent by client
            msgsnd(connector_msg_key, &msg, MSG_DATA_LENGTH, 0);
                
            connect_result = nullptr;
        }   
    }

#endif

    void CreateNewRootSession(Session*& new_session, string ip, int port)
    {
        // create new session
        new_session = new Session();
        #if defined(PLATFORM_IS_MAC)
            new_session->msg_queue_id = msg_queue_id;
        #endif
        #if defined(PLATFORM_IS_WIN)
            new_session->pipe_name = pipe_name;
        #endif
        new_session->client_ip = ip;
        new_session->client_port = port;
        new_session->connect_db_name = DEFAULT_DB_FOLDER_NAME;
        new_session->connector_identity = ROOT;
        
        // get cache db from db_worker_thread
        new_session->cached_db = session_map[DEFAULT_DB_FOLDER_NAME]->cached_db;

        // store session
        sessions.push_back(new_session);
        session_map[ip + std::to_string(port)] = new_session;

        // set satate
        new_session->connect_state = true;

        // create new worker, start the worker thread.
        std::thread new_worker_thread([this, new_session]{
            this->worker_map[new_session] = new Worker(new_session, this->db_operator, session_map[DEFAULT_DB_FOLDER_NAME]->cached_db);
            cout << "Worker: " << new_session->cached_db->db_name << " run successfully on: " << std::this_thread::get_id() << std::endl;
            this->worker_map[new_session]->RootIdentityListenThread();
            });
        new_worker_thread.detach();
    }

    void CreateNewSession(Session*& new_session, string ip, int port, UserIdentity identity, string db_name)
    {
        // create new session
        new_session = new Session();
        new_session->msg_queue_id = msg_queue_id;
        new_session->client_ip = ip;
        new_session->client_port = port;
        new_session->connect_db_name = db_name;
        new_session->connector_identity = identity;
        
        // open db
        new_session->cached_db = new DB();
        new_session->cached_db->db_name = db_name;
        if (!db_operator->OpenDB(session_map[DEFAULT_DB_FOLDER_NAME]->cached_db, new_session->cached_db))
        {
            new_session->connect_state = false;
            return;
        }
        
        // store session
        sessions.push_back(new_session);
        session_map[ip + std::to_string(port)] = new_session;

        // set satate
        new_session->connect_state = true;

        // create new worker, start the worker thread.
        std::thread new_worker_thread([this, new_session]{
            this->worker_map[new_session] = new Worker(new_session, this->db_operator, session_map[DEFAULT_DB_FOLDER_NAME]->cached_db);
            cout << "Worker: " << new_session->cached_db->db_name << " run successfully on: " << std::this_thread::get_id() << std::endl;
            this->worker_map[new_session]->ListenThread();
            });
        new_worker_thread.detach();
    }

    bool GetSession(string ip, int port, Session*& session){
        if (session_map.find(ip + std::to_string(port)) != session_map.end())
        {
            session = session_map[ip + std::to_string(port)];
            return true;
        }
        return false;
    }

    void ShutDown()
    {
        working = false;
    }
};

}

#endif // VDBMS_BENCH_CONNECTOR_H_