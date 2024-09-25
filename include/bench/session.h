// Copyright (c) 2024 by dingning
//
// file  : session.h
// since : 2024-08-07
// desc  : this session will store information about the connection between client and server. Include follow information:
// 1、user identity: root/user
// 2、connect dbms: all user operation can only change the data belongs the connect dbms; 
// root user can change each dbms except the default dbms, which will store all information about this system
// 3、sql execute state: one user can only submit one sql to execute at one time, so client can watch this state to control the client opertion.

#ifndef VDBMS_BENCH_SESSION_H_
#define VDBMS_BENCH_SESSION_H_

#include <string>
#include <mutex>

#include "../meta/db/db.h"

using std::string;

namespace tiny_v_dbms {


enum UserIdentity
{
    ROOT,
    USER
};

struct Session
{

    // belows fields are used to cache the msg about db and tables, to avoid frequently io.
    DB* cached_db;
    int db_user_amount;
    std::mutex db_mutex;

    // belows fields are used to connect
#if defined(PLATFORM_IS_MAC)
    long msg_queue_id;  // this id will store the queue used to communicate
#endif
#if defined(PLATFORM_IS_WIN)
    std::string pipe_name;  // 使用pipename来指定管道
#endif

    // 客户端信息
    UserIdentity connector_identity;
    std::string connect_db_name;
    std::string client_ip;
    int client_port;

    bool connect_state; // True means this connection is in use and connect success

    void Serialize(char* buffer)
    {
        int offset = 0;

        // Serialize client_ip
        int ip_length = client_ip.length();
        memcpy(buffer + offset, &ip_length, sizeof(int));
        offset += sizeof(int);
        memcpy(buffer + offset, client_ip.c_str(), ip_length);
        offset += ip_length;

        // Serialize client_port
        memcpy(buffer + offset, &client_port, sizeof(int));
        offset += sizeof(int);

        // Serialize connector_identity
        memcpy(buffer + offset, &connector_identity, sizeof(UserIdentity));
        offset += sizeof(UserIdentity);

        // Serialize connect_db_name
        int db_name_length = connect_db_name.length();
        memcpy(buffer + offset, &db_name_length, sizeof(int));
        offset += sizeof(int);
        memcpy(buffer + offset, connect_db_name.c_str(), db_name_length);
        offset += db_name_length;

        // Serialize connect_state
        memcpy(buffer + offset, &connect_state, sizeof(bool));
    }

    void Deserialize(char* buffer)
    {
        int offset = 0;

        // Deserialize client_ip
        int ip_length;
        memcpy(&ip_length, buffer + offset, sizeof(int));
        offset += sizeof(int);
        char* ip = new char[ip_length];
        memcpy(ip, buffer + offset, ip_length);
        client_ip = ip;
        offset += ip_length;

        // Deserialize client_port
        memcpy(&client_port, buffer + offset, sizeof(int));
        offset += sizeof(int);

        // Deserialize connector_identity
        memcpy(&connector_identity, buffer + offset, sizeof(UserIdentity));
        offset += sizeof(UserIdentity);

        // Deserialize connect_db_name
        int db_name_length;
        memcpy(&db_name_length, buffer + offset, sizeof(int));
        offset += sizeof(int);

        char* db_name = new char[db_name_length];
        memcpy(db_name, buffer + offset, db_name_length);
        connect_db_name = db_name;
        offset += db_name_length;

        // Deserialize connect_state
        memcpy(&connect_state, buffer + offset, sizeof(bool));
    }
    
    string Serialize()
    {
        
    }

    // // 下方两个序列化函数用于建立连接前发送客户端连接请求信息
    // void SerializeClientInform(char* buffer)
    // {
    //     int offset = 0;

    //     // Serialize client_ip
    //     int ip_length = client_ip.length();
    //     memcpy(buffer + offset, &ip_length, sizeof(int));
    //     offset += sizeof(int);
    //     memcpy(buffer + offset, client_ip.c_str(), ip_length);
    //     offset += ip_length;

    //     // Serialize client_port
    //     memcpy(buffer + offset, &client_port, sizeof(int));
    //     offset += sizeof(int);

    //     // Serialize connector_identity
    //     memcpy(buffer + offset, &connector_identity, sizeof(UserIdentity));
    //     offset += sizeof(UserIdentity);

    //     // Serialize connect_db_name
    //     int db_name_length = connect_db_name.length();
    //     memcpy(buffer + offset, &db_name_length, sizeof(int));
    //     offset += sizeof(int);
    //     memcpy(buffer + offset, connect_db_name.c_str(), db_name_length);
    //     offset += db_name_length;
    // }

    // void DeserializeClientInform(char* buffer)
    // {
    //     int offset = 0;

    //     // Deserialize client_ip
    //     int ip_length;
    //     memcpy(&ip_length, buffer + offset, sizeof(int));
    //     offset += sizeof(int);
    //     char* ip = new char[ip_length];
    //     memcpy(ip, buffer + offset, ip_length);
    //     client_ip = ip;
    //     offset += ip_length;

    //     // Deserialize client_port
    //     memcpy(&client_port, buffer + offset, sizeof(int));
    //     offset += sizeof(int);

    //     // Deserialize connector_identity
    //     memcpy(&connector_identity, buffer + offset, sizeof(UserIdentity));
    //     offset += sizeof(UserIdentity);

    //     // Deserialize connect_db_name
    //     int db_name_length;
    //     memcpy(&db_name_length, buffer + offset, sizeof(int));
    //     offset += sizeof(int);

    //     char* db_name = new char[db_name_length];
    //     memcpy(db_name, buffer + offset, db_name_length);
    //     connect_db_name = db_name;
    //     offset += db_name_length;
    // }

    void Close()
    {
        delete cached_db;
    }
};

/**
 * @brief: win平台下建立连接使用的结构体，由客户端向服务器发送
 */
struct ConnectionRequest
{
    // 客户端信息
    int connect_identity_type;
    std::string connect_db_name;
    std::string client_ip;
    int client_port;

    void Serialize(char* buffer) {
        int offset = 0;

        // 序列化客户端信息
        memcpy(buffer + offset, &connect_identity_type, sizeof(int));
        offset += sizeof(int);

        int db_name_length = connect_db_name.length();
        memcpy(buffer + offset, &db_name_length, sizeof(int));
        offset += sizeof(int);
        memcpy(buffer + offset, connect_db_name.c_str(), db_name_length);
        offset += db_name_length;

        int ip_length = client_ip.length();
        memcpy(buffer + offset, &ip_length, sizeof(int));
        offset += sizeof(int);
        memcpy(buffer + offset, client_ip.c_str(), ip_length);
        offset += ip_length;

        memcpy(buffer + offset, &client_port, sizeof(int));
        offset += sizeof(int);
    }

    void Deserialize(char* buffer) {
        int offset = 0;

        // 反序列化客户端信息
        memcpy(&connect_identity_type, buffer + offset, sizeof(int));
        offset += sizeof(int);

        int db_name_length;
        memcpy(&db_name_length, buffer + offset, sizeof(int));
        offset += sizeof(int);
        connect_db_name = std::string(buffer + offset, db_name_length);
        offset += db_name_length;

        int ip_length;
        memcpy(&ip_length, buffer + offset, sizeof(int));
        offset += sizeof(int);
        client_ip = std::string(buffer + offset, ip_length);
        offset += ip_length;

        memcpy(&client_port, buffer + offset, sizeof(int));
        offset += sizeof(int);
    }
};

/**
 * @brief: win平台下建立连接的返回结果
 */
struct ConnectionResult
{
    bool state; 
    std::string inform;

    /**
     * 序列化，将 ConnectionResult 的内容写入到缓冲区 buffer 中
     * @param buffer 写入的位置
     */
    void Serialize(char* buffer)
    {
        // 序列化 bool (1字节)
        buffer[0] = state ? 1 : 0;

        // 序列化 string 的长度 (4字节)
        size_t informLength = inform.size();
        memcpy(buffer + 1, &informLength, sizeof(informLength));

        // 序列化 string 的内容
        memcpy(buffer + 1 + sizeof(informLength), inform.c_str(), informLength);
    }

    /**
     * 反序列化，从缓冲区 buffer 中读取 ConnectionResult 的内容
     */
    void Deserialize(char* buffer) 
    {
        // 反序列化 bool
        state = buffer[0] != 0;

        // 反序列化 string 的长度
        size_t informLength;
        memcpy(&informLength, buffer + 1, sizeof(informLength));

        // 反序列化 string 的内容
        inform.assign(buffer + 1 + sizeof(informLength), informLength);
    }

    size_t Size()
    {
        return sizeof(bool) + sizeof(size_t) + inform.size();
    }

    void ConnectFailure(std::string information)
    {
        state = false;
        inform = information;
    }

    void ConnectSuccess(std::string information)
    {
        state = true;
        inform = information;
    }
};

}

#endif // VDBMS_BENCH_SESSION_H_