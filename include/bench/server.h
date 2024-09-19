// Copyright (c) 2024 by dingning
//
// file  : server.h
// since : 2024-07-TODO
// desc  : the server of this dbms.

#ifndef VDBMS_BENCH_SERVER_H_
#define VDBMS_BENCH_SERVER_H_

#include "./connector.h"

#include "../sql/executer/operator.h"

namespace tiny_v_dbms {

class Server
{

private:
    Operator* op;
    Connector* connector;
    map<Session*, Worker*> worker_map;

public:
    
    Server()
    {

        op = new Operator();

        // if db has not been installed, install
        if (op->Install())
        {
            return; // 终止程序，重启程序后自动连接刚建立的base_db
        }

        connector = new Connector(worker_map, op);
    }

#if defined(PLATFORM_IS_MAC)
    ~Server()
    {
        msgctl(connector_msg_key, IPC_RMID, nullptr);
        msgctl(worker_msg_key, IPC_RMID, nullptr);
        msgctl(base_db_worker_msg_key, IPC_RMID, nullptr);

        Clean();
    }
#endif
#if defined(PLATFORM_IS_WIN)
    ~Server()
    {


        Clean();
    }   
#endif

    void Run()
    {   
        // Run connector
        // Create one thread to handle all request to base_db (create database sql)
        connector->RunBaseDBThread();

        // run forward thread to execute connect request
        connector->RunForwardThread();
    }

    void Clean()
    {
        delete connector;
        delete op;
        for (auto item : worker_map)
        {
            delete item.first;
            delete item.second;
        }
    }
};

}

#endif // VDBMS_BENCH_SERVER_H_