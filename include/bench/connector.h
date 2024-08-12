// Copyright (c) 2024 by dingning
//
// file  : connector.h
// since : 2024-08-09
// desc  : TODO.

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
#include <sys/ipc.h>
#include <sys/msg.h>

#include "../config.h"

using std::vector;
using std::string;
using std::map;
using std::cout;
using std::cin;
using std::endl;


namespace tiny_v_dbms {

enum UserIdentity
{
    ROOT,
    USER
};
struct Session
{
    long msg_queue_id;  // this id will store the queue used to communicate

    UserIdentity connector_identity;
    string connect_db_name;

    string client_ip;
    int client_port;

    bool connect_state; // True means this connection is in use and connect success

    void Serialize(char* buffer) {
        int offset = 0;

        // Serialize client_ip
        int ip_length = client_ip.length();
        memcpy(buffer + offset, &ip_length, sizeof(int));
        offset += sizeof(int);
        memcpy(buffer + offset, client_ip.c_str(), ip_length);
        offset += ip_length;

        // Serialize client_port
        memcpy(buffer + offset, &client_port, sizeof(int));
        offset += sizeof(int);

        // Serialize connector_identity
        memcpy(buffer + offset, &connector_identity, sizeof(UserIdentity));
        offset += sizeof(UserIdentity);

        // Serialize connect_db_name
        int db_name_length = connect_db_name.length();
        memcpy(buffer + offset, &db_name_length, sizeof(int));
        offset += sizeof(int);
        memcpy(buffer + offset, connect_db_name.c_str(), db_name_length);
        offset += db_name_length;

        // Serialize connect_state
        memcpy(buffer + offset, &connect_state, sizeof(bool));
        offset += sizeof(bool);
    }
};
enum SqlState
{
    UNSUBMIT,
    PARSE_ERROR,
    EXECUTING,
    SUCCESS
};
struct SqlResponse
{
    SqlState sql_state;
    string information;

    void Serialize(char* buffer)
    {
        default_address_type offset = 0;

        memcpy(buffer, &sql_state, sizeof(SqlState));
        offset += sizeof(SqlState);

        int information_length = information.length();
        memcpy(buffer + offset, &information_length, sizeof(int));
        offset += sizeof(int);
        
        memcpy(buffer + offset, information.c_str(), information_length);
    }
};

typedef struct ConnectMsg{
    long msg_type;
    char msg_data[MSG_DATA_LENGTH];  // ip[15]:192.192.192.192 | port[4]: int | connect_identity_type[4]: int | connect_db_name[100]: "test_db_name" ｜ state[1]: true
}ConnectMsg;

typedef struct WorkMsg{
    long msg_type;
    char msg_data[WORK_MSG_DATA_LENGTH]; // sql_length[4]: 16 | sql[196]: select * from a;
}WorkMsg;

#ifndef CONNECTOR_MSG_KEY
#define CONNECTOR_MSG_KEY
int connector_msg_key = msgget(CONNECTOR_MESSAGE_KEY, IPC_CREAT | 0755);
#endif // CONNECTOR_MSGID

#ifndef WORKER_MSG_KEY
#define WORKER_MSG_KEY
int worker_msg_key =  msgget(WORKER_MESSAGE_KEY, IPC_CREAT | 0755);
#endif // WORKER_MSG_KEY

class Worker
{
private:
    // cache session
    Session* user_session;
    bool working;

    // information about sql
    string executing_sql;
    SqlResponse* executing_sql_response;

    // thread mutex
    std::mutex param_mutex;
    std::condition_variable param_condVar;
    std::mutex response_mutex;
    std::condition_variable response_condVar;

    std::pair<std::thread*, std::thread*> fw_bg_threads;
    
public:

    Worker(Session* user)
    {
        user_session = user;
        working = true;
    }

    ~Worker()
    {
        working = false;


    }

    void WorkThread()
    {
        while(working)
        {   
            // wait until executing_sql not empty
            std::unique_lock<std::mutex> lock(param_mutex);
            param_condVar.wait(lock);
            
            // execute sql
            cout << std::this_thread::get_id() <<  "input sql: " << executing_sql << endl;
            cout << "input executing_sql_response: " << executing_sql_response << endl;

            // write back result  
            std::unique_lock<std::mutex> response_lock(response_mutex);
            executing_sql_response->sql_state = SqlState::SUCCESS;
            executing_sql_response->information = "success";
            response_lock.release();

            // notify 
            response_condVar.notify_one();
        }
    }

    void ListenThread()
    {
        // check msg queue is ready
        if (worker_msg_key < 0)
        {
            throw std::runtime_error("msg queue not start successfully");
        }
        
        WorkMsg work_msg;
        work_msg.msg_type = user_session->msg_queue_id;

        // read one sql msg from queue
        // first msg data struct: | send back queue id |
        msgrcv(worker_msg_key, &work_msg, MSG_DATA_LENGTH, CONNECTOR_MSG_TYPE_RECV, 0);
            
        // store the send back msg
        long send_back_id;
        memcpy(&send_back_id, work_msg.msg_data, sizeof(long));

        while(working)
        {   
            // receive msg id
            work_msg.msg_type = user_session->msg_queue_id;

            // get sql length
            int sql_length;
            memcpy(&sql_length, work_msg.msg_data, sizeof(sql_length));

            assert(sql_length < SQL_MAX_LENGTH);

            // lock to write sql
            std::unique_lock<std::mutex> lock(param_mutex);
            memcpy(&executing_sql, work_msg.msg_data + sizeof(sql_length), sql_length);

            // release lock on hands
            lock.release();

            // notify work thread
            param_condVar.notify_one();

            // wait until result is ok
            std::unique_lock<std::mutex> response_lock(response_mutex);
            response_condVar.wait(lock);

            // write response to queue
            executing_sql_response->Serialize(work_msg.msg_data);
            work_msg.msg_type = send_back_id;
            msgsnd(connector_msg_key, &work_msg, strlen(work_msg.msg_data) + 1, 0);
        }
    }

    void Shutdown()
    {
        working = false;
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

    // thread mutex
    bool working;
    std::mutex worker_mutex;
    std::mutex result_ready_mutex;
    std::condition_variable worker_condition;
    std::condition_variable result_ready_condition;

    // used to cache connection information and result
    long msg_queue_id; string connect_ip; int connect_port; UserIdentity connect_identity; string connect_db_name; 
    Session* connect_result;

    // store the worker thread, make sure they will end before this object destruct
    std::vector<std::thread> worker_threads;

public:

    Connector(map<Session*, Worker*>& workers)
    {
        working = true;
        worker_map = workers;
    }

    ~Connector()
    {
        // stop all worker threads
        ShutDown();
        for (auto& t : worker_threads) {
            if (t.joinable()) {
                t.join();
            }
        }

        // destory all sessions
        for (auto& item: sessions)
        {
            delete item;
        }
        session_map.clear();
    }

    Session* Connect(string ip, int port, UserIdentity identity, string db_name)
    {
        
        Session* new_session;

        // check exist
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
            // throw std::runtime_error("connection has been created on :" + ip + " : " + std::to_string(port));
        }

        // create new session
        new_session = new Session();
        new_session->msg_queue_id = msg_queue_id;
        new_session->client_ip = ip;
        new_session->client_port = port;
        new_session->connect_db_name = db_name;
        new_session->connector_identity = identity;
        
        // store session
        sessions.push_back(new_session);
        session_map[ip + std::to_string(port)] = new_session;

        // set satate
        new_session->connect_state = true;

        // TODO: create new worker, start the worker thread.
        worker_map[new_session] = 


        return new_session;
    } 

    bool GetSession(string ip, int port, Session*& session){
        if (session_map.find(ip + std::to_string(port)) != session_map.end())
        {
            session = session_map[ip + std::to_string(port)];
            return true;
        }
        return false;
    }

    // this thread will generate sessions
    void RunBackgroundThread()
    {   
        while (working)
        {
            // wait until connection informaiton ready
            std::unique_lock<std::mutex> lock(worker_mutex);
            worker_condition.wait(lock);
            
            // get lock to write connect_result
            std::unique_lock<std::mutex> result_lock(result_ready_mutex);
            connect_result = Connect(connect_ip, connect_port, connect_identity, connect_db_name);

            // wake up reader thread
            result_ready_condition.notify_one();
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

            // get receive queue id and send back queue id
            memcpy(&msg.msg_type, msg.msg_data, sizeof(long));
            long send_back_id;
            memcpy(&send_back_id, msg.msg_data, sizeof(long));
            
            // get one detail message from special message queue(client send information msg to connect server)
            msgrcv(connector_msg_key, &msg, MSG_DATA_LENGTH, CONNECTOR_MSG_TYPE_RECV, 0);
            cout << "receive from client" << msg.msg_data << std::endl;

            // get lock to write connect information
            std::unique_lock<std::mutex> lock(worker_mutex);

            // get connection information from input stream
            msg_queue_id = msg.msg_type;   // cache the special queue id, and it will be used lately between client and the sql worker of the client
            
            default_address_type offset = 0;

            memcpy(&connect_ip, msg.msg_data + offset, IP_LENGTH);
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

            memcpy(&connect_db_name, msg.msg_data + offset, db_name_length);
            
            // release lock on hands
            lock.release();

            // wake up connector thread
            Session* result = CreateConnection();

            // send connect result to sepcial queue
            result->Serialize(msg.msg_data);
            msg.msg_type = send_back_id;
            msgsnd(connector_msg_key, &msg, strlen(msg.msg_data) + 1, 0);

        }   
    }

    Session* CreateConnection()
    {
        // wake up working thread
        worker_condition.notify_one();

        // block this thread until connect been created.
        std::unique_lock<std::mutex> result_lock(result_ready_mutex);
        result_ready_condition.wait(result_lock);

        // then return create result(session)
        return connect_result;
    }

    void ShutDown()
    {
        working = false;
    }
};


class Server
{
    Connector* connector;
    map<Session*, Worker*>* worker_map;

public:
    
    Server()
    {
        connector = new Connector(worker_map);
    }

    void RunConnector()
    {
        auto background_func = std::bind(&Connector::RunBackgroundThread, connector);
        auto forward_func = std::bind(&Connector::RunForwardThread, connector);

        std::thread bg_thread(background_func);
        std::thread fw_thread(forward_func);

        // move the thread ownership
        connector->worker_threads.push_back(std::move(bg_thread));
        connector->worker_threads.push_back(std::move(fw_thread));

        // // thread run in background
        // bg_thread.detach();
        // fw_thread.detach();
    }

    void RunServer()
    {   
        // two thread(background, forward)to run connector
        RunConnector();
    }

    static void ExeSql(string sql, Session* session)
    {
        // execute sql

    }


};

}

#endif // VDBMS_BENCH_CONNECTOR_H_