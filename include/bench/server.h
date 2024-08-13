// Copyright (c) 2024 by dingning
//
// file  : server.h
// since : 2024-07-TODO
// desc  : the server of this dbms.

#ifndef VDBMS_BENCH_SERVER_H_
#define VDBMS_BENCH_SERVER_H_

#include "./connector.h"

namespace tiny_v_dbms {

class Server
{
    Connector* connector;
    map<Session*, Worker*> worker_map;

public:
    
    Server()
    {
        connector = new Connector(worker_map);
    }

    void Run()
    {
        connector->RunForwardThread();
    }

};

}

#endif // VDBMS_BENCH_SERVER_H_