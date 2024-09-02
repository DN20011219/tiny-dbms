// Copyright (c) 2024 by dingning
//
// file  : demo_bench.h
// since : 2024-07-21
// desc  : TODO.

#ifndef VDBMS_BENCH_DEMO_BENCH_H_
#define VDBMS_BENCH_DEMO_BENCH_H_

#include "server.h"

namespace tiny_v_dbms {


    void StartBench() 
    {
        Server server;
        server.Run();
    }



}

#endif // VDBMS_BENCH_DEMO_BENCH_H_