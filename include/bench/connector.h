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
    UserIdentity connector_identity;
    string connect_db_name;

    string client_ip;
    int client_port;
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
};


class Client
{

private:
    string client_ip;
    int client_port;

public:

    static void Connect(string server_ip, int server_port, string db_name)
    {

    }

    static void RunClient()
    {

    }   

};

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

    // test
    int exe_sql_num;
    
public:

    Worker(Session* user)
    {
        user_session = user;
        working = true;
        exe_sql_num = 0;
    }

    ~Worker()
    {
        delete user_session;
    }

    void WorkThread()
    {
        while(working)
        {   
            // wait until executing_sql not empty
            std::unique_lock<std::mutex> lock(param_mutex);
            param_condVar.wait(lock, [this]{return this->executing_sql != "";});
            
            // execute sql
            cout << std::this_thread::get_id() <<  "input sql: " << executing_sql << endl;
            cout << "input executing_sql_response: " << executing_sql_response << endl;
            exe_sql_num++;
            cout << std::this_thread::get_id() << "exe_sql_num: " << exe_sql_num << endl;

            // write back result  
            std::unique_lock<std::mutex> response_lock(response_mutex);
            executing_sql_response->sql_state = SqlState::SUCCESS;
            executing_sql_response->information = "success";
            // notify 
            response_condVar.notify_one();
        }
    }

    void SqlThread(string sql, SqlResponse* response)
    {
        // lock to write sql information
        std::unique_lock<std::mutex> lock(param_mutex);
        executing_sql = sql;
        executing_sql_response = response;
        // notify work thread
        param_condVar.notify_one();

        // wait until result is ok
        std::unique_lock<std::mutex> response_lock(response_mutex);
        response_condVar.wait(lock);
    }

    void Shutdown()
    {
        working = false;
    }

};


class Connector
{

private:
    // store sessions
    vector<Session*> sessions;
    map<string, Session*> session_map;

    // cache the sessions - worker map from Server
    map<Session*, Worker*>* worker_map;

    // thread mutex
    bool working;
    std::mutex worker_mutex;
    std::mutex result_ready_mutex;
    std::condition_variable worker_condition;
    std::condition_variable result_ready_condition;

    // used to cache connection information and result
    string connect_ip; int connect_port; UserIdentity connect_identity; string connect_db_name; 
    Session* connect_result;


public:

    Connector()
    {
        working = true;
    }

    ~Connector()
    {
        ShutDown();

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
            throw std::runtime_error("connection has been created on :" + ip + " : " + std::to_string(port));
        }

        // create new session
        new_session = new Session();
        new_session->client_ip = ip;
        new_session->client_port = port;
        new_session->connect_db_name = db_name;
        new_session->connector_identity = identity;
        
        // store session
        sessions.push_back(new_session);
        session_map[ip + std::to_string(port)] = new_session;

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
    
    void RunForwardThread()
    {
        while (working)
        {
            // get lock to write connect information
            std::unique_lock<std::mutex> lock(worker_mutex);

            // get connection information from input stream
            cin >> connect_ip;
            cin >> connect_port;
            int connect_identity_type;
            cin >> connect_identity_type;
            connect_identity = UserIdentity(connect_identity_type);
            cin >> connect_db_name;

            // create connection
            CreateConnection();
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
        delete this;
    }

};


class Server
{

    Connector* connector;
    map<Session*, Worker*> worker_map;


public:
    
    Server()
    {
        connector = new Connector();
    }

    void RunConnector()
    {
        auto background_func = std::bind(&Connector::RunBackgroundThread, connector);
        auto forward_func = std::bind(&Connector::RunForwardThread, connector);

        std::thread bg_thread(background_func);
        std::thread fw_thread(forward_func);

        // thread run in background
        bg_thread.detach();
        fw_thread.detach();
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