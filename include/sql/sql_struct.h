// Copyright (c) 2024 by dingning
//
// file  : sql_struct.h
// since : 2024-08-13
// desc  : this file contains all struct about sql execution.

#ifndef VDBMS_SQL_EXECUTER_SQL_STRUCT_H_
#define VDBMS_SQL_EXECUTER_SQL_STRUCT_H_

#include <string>

#include "../bench/session.h"
#include "../meta/value.h"
#include "../config.h"

using std::string;

namespace tiny_v_dbms {

enum SqlState
{
    UNSUBMIT,
    PARSE_ERROR,
    SUCCESS,
    FAILURE
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

        uint32_t sql_state_value = static_cast<uint32_t>(sql_state);
        memcpy(buffer, &sql_state_value, sizeof(uint32_t));
        buffer += sizeof(uint32_t);

        int information_length = information.length();
        memcpy(buffer + offset, &information_length, sizeof(int));
        offset += sizeof(int);
        
        if (information_length > 0)
            memcpy(buffer + offset, information.c_str(), information_length);
    }

    void Deserialize(char* buffer)
    {
        default_address_type offset = 0;

        // memcpy(&sql_state, buffer + offset, sizeof(SqlState));
        // offset += sizeof(SqlState);
        uint32_t sql_state_value = static_cast<uint32_t>(sql_state);
        memcpy(&sql_state_value, buffer + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        sql_state = SqlState(sql_state_value);

        int information_length = information.length();
        memcpy(&information_length, buffer + offset, sizeof(int));
        offset += sizeof(int);
        
        if (information_length > 0)
        {
            char* inform = new char[information_length];
            memcpy(inform, buffer + offset, information_length);
            information.assign(inform);
            delete[] inform;
        } else {
            information.clear(); // Initialize information to an empty string
        }
    }
};

// means this base node is used to store which type of sql.
enum NodeType {
    CREATE_DATABASE_NODE,
    CREATE_TABLE_NODE,
    INSERT_INTO_TABLE_NODE,
    SELECT_FROM_ONE_TABLE_NODE,
    DROP_DATABASE_NODE,
    DROP_TABLE_NODE,

    UNSUPPORT_NODE
};

enum IndexType
{
    NONE_INDEX,
    B_PLUS_TREE,
    UNIQUE,
    VECTOR_INDEX_1, // this type of index will not been finished in 0.0.1 version
};

struct DataBase
{
    string db_name;
};

struct Column
{
    string col_name;
    ValueType value_type;
    int col_length;
};

struct Index
{
    string index_name;
    string col_nname;
    IndexType index_type;
};


enum Comparator
{
    BIGGER,
    LESS,
    EQUAL,
    NOT_EQUAL
};

Comparator SwitchComparator(string value)
{
    if (value == ">")
    {
        return Comparator::BIGGER;
    } 
    else if (value == "<")
    {
        return Comparator::LESS;
    } 
    else if (value == "=")
    {
        return Comparator::EQUAL;
    } 
    else if (value == "!=")
    {
        return Comparator::NOT_EQUAL;
    } 
    else 
    {
        throw std::runtime_error("value: " + value + " can not be parsed to Comparator");
    }
}

struct CompareCondition  // such as a = b, a != b, a > b, a < b
{
    Column col;
    Comparator condition;
    string compare_value;
};

enum OperatorEnum
{
    AND,
    OR
};
OperatorEnum SwitchOperator(string value)
{
    if (value == "AND")
    {
        return OperatorEnum::AND;
    } 
    else if (value == "OR")
    {
        return OperatorEnum::OR;
    } 
    else 
    {
        throw std::runtime_error("value: " + value + " can not be parsed to Operator");
    }
}
struct Operation
{
    CompareCondition left_leaf;
    Operation* left_op;
    OperatorEnum opreator;
    CompareCondition right_leaf;
};

}

#endif // VDBMS_SQL_EXECUTER_SQL_STRUCT_H_