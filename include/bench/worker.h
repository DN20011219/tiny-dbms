// Copyright (c) 2024 by dingning
//
// file  : worker.h
// since : 2024-07-TODO
// desc  : this class will handle the sql and return execute result to client.

#ifndef VDBMS_BENCH_WORKER_H_
#define VDBMS_BENCH_WORKER_H_

#include <iostream>
#include <string>
#include <thread>

#include "./session.h"
#include "./queue_msg.h"
#include "../sql/sql_struct.h"

using std::string;
using std::cout;
using std::endl;

namespace tiny_v_dbms {

class Worker
{
    friend class Connector;

private:
    // cache session
    Session* user_session;
    bool working;

    // information about sql
    string executing_sql;
    SqlResponse* executing_sql_response;
    
public:

    Worker(Session* user)
    {
        user_session = user;
        working = true;

        // auto listen_thread = std::bind(&Worker::ListenThread, this);
        // std::thread fw_thread(listen_thread);
        // ListenThread();
        // cout << "Worker: " << user->connect_db_name << " run successfully on: " << std::this_thread::get_id() << std::endl;
    }

    ~Worker()
    {
        working = false;

        // delete pointer resource
        if (executing_sql_response != nullptr)
            delete executing_sql_response;
    }

    void Work()
    {
        // execute sql
        cout << "input sql: " << executing_sql << endl;
        
        // write back result  
        executing_sql_response = new SqlResponse();
        executing_sql_response->sql_state = SqlState::SUCCESS;
        executing_sql_response->information = "success";

         cout << "input executing_sql_response: " << executing_sql_response->information << endl;
    }

    void ListenThread()
    {
        // check msg queue is ready
        if (worker_msg_key < 0)
        {
            throw std::runtime_error("worker_msg_key queue not start successfully");
        }
        
        cout << "worker_msg_key: " << worker_msg_key << endl;

        WorkMsg work_msg;
        long send_back_id = user_session->msg_queue_id + 1; // store the send back queue id

        while(working)
        {   
            // receive msg id
            work_msg.msg_type = user_session->msg_queue_id; // use special receive queue id
            msgrcv(worker_msg_key, &work_msg, WORK_MSG_DATA_LENGTH, user_session->msg_queue_id, 0);

            // get sql length
            int sql_length;
            memcpy(&sql_length, work_msg.msg_data, sizeof(sql_length));
            assert(sql_length < SQL_MAX_LENGTH);

            // store sql
            char* sql_cache = new char[sql_length];
            memcpy(sql_cache, work_msg.msg_data + sizeof(sql_length), sql_length);
            executing_sql.assign(sql_cache);
            delete[] sql_cache;

            // execute sql
            Work();

            // write response to queue
            executing_sql_response->Serialize(work_msg.msg_data);
            work_msg.msg_type = send_back_id;
            msgsnd(worker_msg_key, &work_msg, executing_sql_response->GetLength(), 0);

            // free resource
            delete executing_sql_response;
            executing_sql_response = nullptr;
        }
    }


};


}

#endif // VDBMS_BENCH_WORKER_H_