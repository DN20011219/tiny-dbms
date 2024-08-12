// Copyright (c) 2024 by dingning
//
// file  : client.h
// since : 2024-08-09
// desc  : TODO.

#ifndef TODO
#define TODO

#include <string>
#include "./connector.h"

namespace tiny_v_dbms {
    
class Client
{

private:
    std::string ip;
    int port;

public:
    Client(Server* server)
    {
        // 尝试调用server.connector连接服务器

        // 启动一个新的命令行作为客户端

        // 循环获取输入作为sql，调用server执行sql。
    }
};


}

#endif // TODO