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
    Connector* connector;
    map<Session*, Worker*> worker_map;

    Operator* op;

public:
    
    Server()
    {
        // if db has not been installed, install
        op = new Operator();
        if (op->Install())
        {
            // 终止程序，要求重启
        }

        connector = new Connector(worker_map, op);
    }

    void Run()
    {   
        // Run connector
        // Create one thread to handle all request to base_db (create database sql)
        connector->CreateNewSession();
        connector->RunForwardThread();
    }

};

}

#endif // VDBMS_BENCH_SERVER_H_