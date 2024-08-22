// Copyright (c) 2024 by dingning
//
// file  : client.h
// since : 2024-08-09
// desc  : This is the client, can send sql to server, and show the execute result.

#ifndef VDBMS_BENCH_CLIENT_H_
#define VDBMS_BENCH_CLIENT_H_

#include <string>
#include <iostream>
#include <thread>
#include <unistd.h>

#include "../config.h"
#include "./queue_msg.h"
#include "../sql/sql_struct.h"

using std::cin;

namespace tiny_v_dbms {

const long queue_list_range[] = {1, 3, 5, 7, 9};  // skip 1, because of the msg sent by server use this queue id + 1

class Client
{

public:

    void RunClient(std::string ip, int port, int connect_identity_type, std::string db_name, long communicate_queue_id)
    {
        if (connector_msg_key < 0)
        {
            throw std::runtime_error("msg queue not start successfully");
        }
        std::cout << "connector_msg_key: " << connector_msg_key << std::endl;

        ConnectMsg msg;

        // set information about the first msg(send_back_queue_id and queue_id)
        msg.msg_type = CONNECTOR_MSG_TYPE_RECV; // use public queue
        memcpy(msg.msg_data, &communicate_queue_id, sizeof(long)); // special queue id
        msgsnd(connector_msg_key, &msg, MSG_DATA_LENGTH, 0);

        std::cout << "have send msg to: " << CONNECTOR_MSG_TYPE_RECV << "  data is: " << communicate_queue_id << std::endl;

        // set information about the second msg
        msg.msg_type = communicate_queue_id; // use special queue
        msg.Serialize(ip, port, connect_identity_type, db_name); // serialize data
        msgsnd(connector_msg_key, &msg, MSG_DATA_LENGTH, 0);

        std::cout << "have send msg to: " << communicate_queue_id << std::endl;

        // receive connect result
        msg.msg_type = communicate_queue_id + 1; // use special queue
        msgrcv(connector_msg_key, &msg, MSG_DATA_LENGTH, communicate_queue_id + 1, 0);

        std::cout << "have received msg from: " << communicate_queue_id + 1 << std::endl;

        bool state;
        string connect_result = ConnectMsg::DeserializeConnectResult(msg, state);

        // check connection state
        if (!state)
        {   
            throw std::runtime_error("Client connect to server fail! db_name: " + db_name);
        }

        std::cout << "-----------Connect result-----------" << std::endl;
        std::cout << connect_result << std::endl;
        std::cout << "-----------Connect result-----------" << std::endl;


        // handle sql
        if (worker_msg_key < 0)
        {
            throw std::runtime_error("msg queue not start successfully");
        }
        std::cout << "worker_msg_key: " << worker_msg_key << std::endl;
        while(true)
        {
            std::string sql;
            getline(cin, sql);

            if (sql == "quit")
            {
                break;
            }
            assert(sql.length() < SQL_MAX_LENGTH);

            // serialize sql
            int sql_length = sql.length();
            memcpy(msg.msg_data, &sql_length, sizeof(int));
            memcpy(msg.msg_data + sizeof(int), sql.c_str(), sql_length);

            // send sql to server
            msg.msg_type = communicate_queue_id; // use special queue
            msgsnd(worker_msg_key, &msg, sizeof(int) + sql_length, 0);

            // receive sql handle result
            msgrcv(worker_msg_key, &msg, WORK_MSG_DATA_LENGTH, communicate_queue_id + 1, 0); // use special receive queue

            // check execute result
            SqlResponse response;
            response.Deserialize(msg.msg_data);

            // show result
            switch (response.sql_state)
            {
            case UNSUBMIT:
                std::cout << "UNSUBMIT" << std::endl;
                break;
            case PARSE_ERROR:
                std::cout << "PARSE_ERROR" << std::endl;
                break;
            case EXECUTING:
                std::cout << "EXECUTING" << std::endl;
                break;
            case SUCCESS:
                std::cout << "SUCCESS: " << response.information << std::endl;
                break;
            default:
                break;
            }
        }
    }
};

class ClientManager
{

private:
    long used_queue_id;

public:

    void Run()
    {
        used_queue_id = 0;
        while(true)
        {
            std::string command;
            getline(cin, command);

            if (command == "quit")
            {
                break;
            }

            if (command == "root")
            {
                BuildRootClient();
                continue;
            }

            if (command == "user")
            {
                BuildClient();
                continue;
            }
        }
    }

    void BuildRootClient()
    {
        Client* cli = new Client();
        cli->RunClient("192.0.0.0", 0, 0, "base_db", queue_list_range[used_queue_id++]);
        delete cli;
    }

    void BuildDefaultUserClient()
    {
        Client* cli = new Client();
        cli->RunClient("192.0.0.0", 0, 0, "first_db", queue_list_range[used_queue_id++]);
        delete cli;
    }

    // build new client
    void BuildClient()
    {
        std::string ip;
        int port;
        int connect_identity_type;
        std::string db_name;
        long communicate_queue_id;

        std::cout << "Enter IP: ";
        std::cin >> ip;

        std::cout << "Enter port: ";
        std::cin >> port;

        std::cout << "Enter connect identity type: ";
        std::cin >> connect_identity_type;

        std::cout << "Enter database name: ";
        std::cin >> db_name;

        communicate_queue_id = queue_list_range[used_queue_id++];

        Client* cli = new Client();
        cli->RunClient(ip, port, connect_identity_type, db_name, communicate_queue_id);
        delete cli;
    }

};

}

#endif // VDBMS_BENCH_CLIENT_H_