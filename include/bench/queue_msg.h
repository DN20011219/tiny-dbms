// Copyright (c) 2024 by dingning
//
// file  : queue_msg.h
// since : 2024-08-13
// desc  : Define the struct of msg queue's msg.

#ifndef VDBMS_BENCH_QUEUE_MSG_H_
#define VDBMS_BENCH_QUEUE_MSG_H_

#include <string>
#include "../config.h"

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

enum SqlState
{
    UNSUBMIT,
    PARSE_ERROR,
    EXECUTING,
    SUCCESS
};

struct SqlResponse
{
    SqlState sql_state;
    std::string information;

    int GetLength()
    {
        return sizeof(SqlState) + sizeof(int) + information.length();
    }

    void Serialize(char* buffer)
    {
        default_address_type offset = 0;

        memcpy(buffer, &sql_state, sizeof(SqlState));
        offset += sizeof(SqlState);

        int information_length = information.length();
        memcpy(buffer + offset, &information_length, sizeof(int));
        offset += sizeof(int);
        
        memcpy(buffer + offset, information.c_str(), information_length);
    }

    void Deserialize(char* buffer)
    {
        default_address_type offset = 0;

        memcpy(&sql_state, buffer + offset, sizeof(SqlState));
        offset += sizeof(SqlState);

        int information_length = information.length();
        memcpy(&information_length, buffer + offset, sizeof(int));
        offset += sizeof(int);
        
        char* inform = new char[information_length];
        memcpy(inform, buffer + offset, information_length);
        information.assign(inform);
    }
};

typedef struct ConnectMsg{
    long msg_type;
    char msg_data[MSG_DATA_LENGTH];  // ip[15]:192.192.192.192 | port[4]: int | connect_identity_type[4]: int | connect_db_name[100]: "test_db_name" ｜ state[1]: true

    void Serialize(std::string ip, int port, int connect_identity_type, std::string db_name)
    {
        assert(ip.length() <= IP_LENGTH);
        assert(db_name.length() <= CONNECT_DB_NAME_LENGTH);

        default_address_type offset = 0;

        memcpy(msg_data + offset, ip.c_str(), ip.length());
        offset += IP_LENGTH;

        memcpy(msg_data + offset, &port, sizeof(int));
        offset += sizeof(int);

        memcpy(msg_data + offset, &connect_identity_type, sizeof(int));
        offset += sizeof(int);       

        int db_name_length = db_name.length();
        memcpy(msg_data + offset, &db_name_length, sizeof(int));
        offset += sizeof(int); 

        memcpy(msg_data + offset, db_name.c_str(), db_name_length);
    }


}ConnectMsg;

typedef struct WorkMsg{
    long msg_type;
    char msg_data[WORK_MSG_DATA_LENGTH]; // sql_length[4]: 16 | sql[196]: select * from a;
}WorkMsg;

}

#endif // VDBMS_BENCH_QUEUE_MSG_H_