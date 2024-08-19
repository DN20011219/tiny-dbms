// Copyright (c) 2024 by dingning
//
// file  : queue_msg.h
// since : 2024-08-13
// desc  : Define the struct of msg queue's msg.

#ifndef VDBMS_BENCH_QUEUE_MSG_H_
#define VDBMS_BENCH_QUEUE_MSG_H_

#include <string>

#include "./session.h"
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

    void static SerializeConnectResult(Session* connect_result, ConnectMsg& msg) {
        default_length_size result_offset = 0;

        // set connect state
        memcpy(msg.msg_data + result_offset, &connect_result->connect_state, sizeof(bool));
        result_offset += sizeof(bool);

        // set table_amount
        int table_amount = connect_result->cached_db->tables.size();
        memcpy(msg.msg_data + result_offset, &table_amount, sizeof(int));
        result_offset += sizeof(int);

        // set tables view str length
        string table_name_view;
        default_length_size table_name_view_str_length = 0;
        // set db information: tables name
        for (int i = 0; i < table_amount; i++) {
            table_name_view_str_length += connect_result->cached_db->tables[i].table_name.length() + 3;
            if (result_offset + table_name_view_str_length > MSG_DATA_LENGTH) {
                break;
            }
            table_name_view += connect_result->cached_db->tables[i].table_name + " | ";
        }
        memcpy(msg.msg_data + result_offset, &table_name_view_str_length, sizeof(default_length_size));
        result_offset += sizeof(default_length_size);
        memcpy(msg.msg_data + result_offset, table_name_view.c_str(), table_name_view_str_length);
        result_offset += table_name_view_str_length;
    }

    std::string static DeserializeConnectResult(ConnectMsg& msg, bool& state) {
        std::string connect_result;

        default_length_size result_offset = 0;

        // get connect state
        memcpy(&state, msg.msg_data + result_offset, sizeof(bool));
        result_offset += sizeof(bool);
        connect_result += "Connect State: " + std::to_string(state) + "\n";

        // get table_amount
        int table_amount;
        memcpy(&table_amount, msg.msg_data + result_offset, sizeof(int));
        result_offset += sizeof(int);
        connect_result += "Table Amount: " + std::to_string(table_amount) + "\n";

        // get tables view str length
        default_length_size table_name_view_str_length;
        memcpy(&table_name_view_str_length, msg.msg_data + result_offset, sizeof(default_length_size));
        result_offset += sizeof(default_length_size);

        // get tables view str
        char* table_name_view_str = new char[table_name_view_str_length + 1];
        memcpy(table_name_view_str, msg.msg_data + result_offset, table_name_view_str_length);
        table_name_view_str[table_name_view_str_length] = '\0'; // null-terminate the string
        result_offset += table_name_view_str_length;
        connect_result += "Table Names: " + std::string(table_name_view_str) + "\n";

        delete[] table_name_view_str;

        return connect_result;
    }

}ConnectMsg;

typedef struct WorkMsg{
    long msg_type;
    char msg_data[WORK_MSG_DATA_LENGTH]; // sql_length[4]: 16 | sql[196]: select * from a;
}WorkMsg;

}

#endif // VDBMS_BENCH_QUEUE_MSG_H_