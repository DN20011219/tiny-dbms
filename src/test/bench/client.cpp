// Copyright (c) 2024 by dingning
//
// file  : client.cpp
// since : 2024-08-09
// desc  : This is the client, can send sql to server, and show the execute result.

#include "../../../include/bench/client.h"

int main()
{
    tiny_v_dbms::ClientManager cm;
    cm.Run();

    return 0;
}
