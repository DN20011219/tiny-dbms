// Copyright (c) 2024 by dingning
//
// file  : sql_struct.h
// since : 2024-08-13
// desc  : this file contains all struct about sql execution.

#ifndef VDBMS_SQL_EXECUTER_SQL_STRUCT_H_
#define VDBMS_SQL_EXECUTER_SQL_STRUCT_H_

#include <string>

#include "../bench/session.h"
#include "../config.h"

using std::string;

namespace tiny_v_dbms {

enum SqlState
{
    UNSUBMIT,
    PARSE_ERROR,
    EXECUTING,
    SUCCESS
};

struct SqlCommit
{
    string sql;
    Session* session;
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

}

#endif // VDBMS_SQL_EXECUTER_SQL_STRUCT_H_