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