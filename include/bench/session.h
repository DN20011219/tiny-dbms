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

using std::string;

namespace tiny_v_dbms {


enum UserIdentity
{
    ROOT,
    USER
};

struct Session
{

    long msg_queue_id;  // this id will store the queue used to communicate

    UserIdentity connector_identity;
    std::string connect_db_name;

    std::string client_ip;
    int client_port;

    bool connect_state; // True means this connection is in use and connect success

    void Serialize(char* buffer) {
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
};


}

#endif // VDBMS_BENCH_SESSION_H_