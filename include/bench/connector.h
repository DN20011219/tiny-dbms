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
#include <sys/ipc.h>
#include <sys/msg.h>

#include "../config.h"
#include "./queue_msg.h"
#include "./worker.h"
#include "./server.h"

using std::vector;
using std::string;
using std::map;
using std::cout;
using std::cin;
using std::endl;


namespace tiny_v_dbms {


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
    long msg_queue_id; string connect_ip; int connect_port; UserIdentity connect_identity; string connect_db_name; 
    Session* connect_result;

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

        // create new worker, start the worker thread.
        worker_map[new_session] = new Worker(new_session);

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
    
    // this thread will watch the msg queue and serialize the information about connect 
    void RunForwardThread()
    {
        // check msg queue is ready
        if (connector_msg_key < 0)
        {
            throw std::runtime_error("msg queue not start successfully");
        }
        cout << "connector_msg_key: " << connector_msg_key << endl;
        cout << "worker_msg_key: " << worker_msg_key << endl;

        ConnectMsg msg;
        msg.msg_type = CONNECTOR_MESSAGE_ID;
        while(working){
            // get one call msg from public queue, contains one unique queue id used to communicate.
            // first msg data struct: | receive queue id | send back queue id |
            msgrcv(connector_msg_key, &msg, MSG_DATA_LENGTH, CONNECTOR_MSG_TYPE_RECV, 0);
            if (!working) break;
            // get receive queue id and send back queue id
            memcpy(&msg.msg_type, msg.msg_data, sizeof(long));
            long special_queue_id;
            memcpy(&special_queue_id, msg.msg_data, sizeof(long));
            
            // get one detail message from special message queue(client send information msg to connect server)
            msgrcv(connector_msg_key, &msg, MSG_DATA_LENGTH, special_queue_id, 0);
            if (!working) break;

            {
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

                // send connect result to sepcial queue
                memcpy(msg.msg_data, &connect_result->connect_state, sizeof(bool));

                msg.msg_type = special_queue_id + 1; // send back use send_back_queue_id + 1, because this queue has been used to receive information msg sent by client
                msgsnd(connector_msg_key, &msg, strlen(msg.msg_data) + 1, 0);
                
                connect_result = nullptr;
            }
        }   
    }

    void ShutDown()
    {
        working = false;
    }
};

}

#endif // VDBMS_BENCH_CONNECTOR_H_