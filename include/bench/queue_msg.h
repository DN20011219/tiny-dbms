// Copyright (c) 2024 by dingning
//
// file  : queue_h
// since : 2024-08-13
// desc  : Define the struct of msg queue's 

#ifndef VDBMS_BENCH_QUEUE_MSG_H_
#define VDBMS_BENCH_QUEUE_MSG_H_

#include <string>

#include "./session.h"
#include "../config.h"

namespace tiny_v_dbms {

/**
 * @brief: 这个结构体用于管道&消息通道的序列化数据的储存，仅仅用于建立连接及返回建立结果
 */
typedef struct ConnectMsg{
    long msg_type;
    char msg_data[MSG_DATA_LENGTH];

    // 将Session中的连接信息序列化至msg_data
    void SerializeConnectResult(Session* connect_result) {
        default_length_size result_offset = 0;

        // set connect state
        memcpy(msg_data + result_offset, &connect_result->connect_state, sizeof(bool));
        result_offset += sizeof(bool);
        if (!connect_result->connect_state)
        {
            return;
        }

        // set table_amount
        int table_amount = connect_result->cached_db->tables.size();
        memcpy(msg_data + result_offset, &table_amount, sizeof(int));
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
        memcpy(msg_data + result_offset, &table_name_view_str_length, sizeof(default_length_size));
        result_offset += sizeof(default_length_size);
        memcpy(msg_data + result_offset, table_name_view.c_str(), table_name_view_str_length);
        result_offset += table_name_view_str_length;
    }

    // 将msg_data中的信息序列化为state和返回的string
    std::string DeserializeConnectResult(bool& state) {
        std::string connect_result;

        default_length_size result_offset = 0;

        // get connect state
        memcpy(&state, msg_data + result_offset, sizeof(bool));
        result_offset += sizeof(bool);
        connect_result += "Connect State: " + std::to_string(state) + "\n";
        if (!state)
        {
            return connect_result;
        }

        // get table_amount
        int table_amount;
        memcpy(&table_amount, msg_data + result_offset, sizeof(int));
        result_offset += sizeof(int);
        connect_result += "Table Amount: " + std::to_string(table_amount) + "\n";

        // get tables view str length
        default_length_size table_name_view_str_length;
        memcpy(&table_name_view_str_length, msg_data + result_offset, sizeof(default_length_size));
        result_offset += sizeof(default_length_size);

        // get tables view str
        char* table_name_view_str = new char[table_name_view_str_length];
        memcpy(table_name_view_str, msg_data + result_offset, table_name_view_str_length);
        result_offset += table_name_view_str_length;
        connect_result += "Table Names: " + std::string(table_name_view_str);

        delete[] table_name_view_str;

        return connect_result;
    }

}ConnectMsg;

typedef struct WorkMsg{
    long msg_type;
    char msg_data[WORK_MSG_DATA_LENGTH]; // sql_length[4]: 16 | sql[1996]: select * from a;

    void DeserializeCreateDropDBMessage(bool& is_create, string& db_name) {
        // check command type
        memcpy(&is_create, msg_data, sizeof(bool));

        // get create/drop db name
        default_length_size name_length;
        memcpy(&name_length, msg_data + sizeof(bool), sizeof(default_length_size));
        assert(name_length < SQL_MAX_LENGTH);

        // store sql
        char* db_name_c = new char[name_length];
        memcpy(db_name_c, msg_data + sizeof(bool) + sizeof(default_length_size), name_length);
        db_name.assign(db_name_c, name_length);
        delete[] db_name_c;
    }

    void SerializeCreateDropDBMessage(bool is_create, const string& db_name) {
        // serialize command type
        memcpy(msg_data, &is_create, sizeof(bool));

        // serialize db name length
        default_length_size name_length = db_name.size();
        memcpy(msg_data + sizeof(bool), &name_length, sizeof(default_length_size));

        // serialize db name
        memcpy(msg_data + sizeof(bool) + sizeof(default_length_size), db_name.c_str(), name_length);
    }

}WorkMsg;

}

#endif // VDBMS_BENCH_QUEUE_MSG_H_