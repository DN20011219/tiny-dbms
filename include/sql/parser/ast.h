// Copyright (c) 2024 by dingning
//
// file  : condition_node.h
// since : 2024-07-26
// desc  : this is a Abstract Syntax Tree(AST) node, means here are sqls to execute.

#ifndef VDBMS_SQL_PASRSER_AST_H_
#define VDBMS_SQL_PASRSER_AST_H_

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

#include "../../meta/value.h"
#include "../../config.h"
#include "token.h"

using std::vector;
using std::string;
using std::stringstream;


namespace tiny_v_dbms {

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
    B_PLUS_TREE,
    UNIQUE,
    VECTOR_INDEX_1 // this type of index will not been finished in 0.0.1 version
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
struct Compare  // such as a == b, a <> b, a > b
{
    void* left_leaf;
    Comparator condition;
    void* right_leaf;
};

enum SetOperator
{
    AND,
    OR
};
struct SetOperation
{
    void* left_leaf;
    SetOperator condition;
    void* right_leaf;
};

// sql struct 
class Sql
{
    friend class AST;
};

class CreateDatabaseSql : public Sql
{
    friend class AST;
private:   
    string db_name;

public:
    // extract information from tokens
    CreateDatabaseSql(vector<Token> tokens)
    {   
        // CREATE DATABASE db_name ;
        db_name = tokens[2].value;
    }
};
class CreateTableSql : public Sql
{
    friend class AST;
private:  
    string table_name;
    vector<Column> columns;

public:
    // extract information from tokens
    CreateTableSql(vector<Token> tokens)
    {   
        // example sql : 
        // CREATE TABLE table_name ;
        // CREATE TABLE table_name (col_name1 col_type , col_name2 col_type2 ... );

        // set table_name;
        table_name = tokens[2].value;

        // deserialize column data
        Column* cache_column = nullptr;
        for (int i = 4; i < tokens.size(); i++)
        {
            // if find ), then break
            if (tokens[i].type == TokenType::OPERATOR_T && tokens[i].value == ")")
            {
                break;
            }
            // skip ,
            if (tokens[i].type == TokenType::OPERATOR_T && tokens[i].value == ",")
            {
                continue;
            }
            // build one new column object, and set it's col_name, value_type and col_length
            if (cache_column == nullptr)
            {   
                // first value is col_name
                cache_column = new Column();
                cache_column->col_name = tokens[i].value;
            }
            else
            {
                // second value is value_type
                cache_column->value_type = GetValueTypeFromStr(tokens[i].value);
                columns.push_back(*cache_column);

                // then set pointer to null, means the next column will begin
                cache_column = nullptr;
            }
        }
    }
};
class InsertIntoTableSql : public Sql
{
    friend class AST;
private:  
    string table_name;
    vector<Column> columns;
    vector<Value> values;

public:
     // extract information from tokens
    InsertIntoTableSql(vector<Token> tokens)
    {   
        // example sql : 
        // INSERT INTO table_name (col_name_1, col_name2, ...) VALUES (value1, value2, ...);

        // set table_name;
        table_name = tokens[2].value;

        // deserialize column data
        int token_flag = 4;
        while(token_flag < tokens.size())
        {
            // if find ), then break
            if (tokens[token_flag].type == TokenType::OPERATOR_T && tokens[token_flag].value == ")")
            {
                break;
            }
            // skip ,
            if (tokens[token_flag].type == TokenType::OPERATOR_T && tokens[token_flag].value == ",")
            {
                token_flag++;
                continue;
            }
            // build one new column object, and set it's col_name
            Column* cache_column = new Column();
            cache_column->col_name = tokens[token_flag].value;
            columns.push_back(*cache_column);
            token_flag++;
        }

        // skip ) VALUES (
        token_flag += 3;

        // deserialize values
        while(token_flag < tokens.size())
        {
            // if find ), then break
            if (tokens[token_flag].type == TokenType::OPERATOR_T && tokens[token_flag].value == ")")
            {
                break;
            }
            // skip ,
            if (tokens[token_flag].type == TokenType::OPERATOR_T && tokens[token_flag].value == ",")
            {
                token_flag++;
                continue;
            }

            // set raw value and add to vector
            Value value(tokens[token_flag].value);
            values.push_back(value);
            token_flag++;
        }
    }
};
// now not support () and nest select
// only support easy select such as: select a, b, c from table_1 where e > 2 AND e = 2 OR a <> "test";
class SelectFromOneTableSql : public Sql
{
    friend class AST;
private:  
    string table_name;
    vector<Column> columns;
    vector<Compare> compare_vector;
    vector<SetOperation> set_operation_vector;

public:
    // TODO:
};
// TODO:Select using join
// struct SelectFromTableWithJoinSql
// {
//     string table_name;
//     vector<Column> columns;
// };
struct DropDatabaseSql : public Sql
{
    friend class AST;
private:  
    string db_name;

public:
    DropDatabaseSql(vector<Token> tokens);
};
struct DropTableSql : public Sql
{
    friend class AST;
private:  
    string table_name;

public:
    DropTableSql(vector<Token> tokens);
};

class AST
{
private:
    NodeType node_type;

    CreateDatabaseSql* create_database_sql;
    CreateTableSql* create_table_sql;
    InsertIntoTableSql* insert_into_table_sql;
    DropDatabaseSql* drop_database_sql;
    DropTableSql* drop_table_sql;
    SelectFromOneTableSql* select_from_one_table_sql;
    // SelectFromTableWithJoinSql* select_from_table_with_join_sql;
    
public:

    ~AST()
    {
        // free sql memory
        switch (node_type)
        {
        case CREATE_DATABASE_NODE:
            delete create_database_sql;
            break;
        case CREATE_TABLE_NODE:
            delete create_table_sql;
            break;
        case INSERT_INTO_TABLE_NODE:
            delete insert_into_table_sql;
            break;
        // TODO: more sql support is on designing
        default:
            break;
        }
    }

    AST(NodeType type, void* data) 
        : node_type(type)
    {
        switch (type)
        {
            case NodeType::CREATE_DATABASE_NODE:
                create_database_sql = static_cast<CreateDatabaseSql*>(data);
                break;
            case NodeType::CREATE_TABLE_NODE:
                create_table_sql = static_cast<CreateTableSql*>(data);
                break;
            case NodeType::INSERT_INTO_TABLE_NODE:
                insert_into_table_sql = static_cast<InsertIntoTableSql*>(data);
                break;
            default:
                throw std::runtime_error("can not init AST!");
        }
    }

    NodeType GetType()
    {
        return node_type;
    }

    string ToString()
    {
        stringstream ss;
        switch (node_type)
        {
        case CREATE_DATABASE_NODE:
        {
            ss << "CREATE DATABASE " << create_database_sql->db_name << ";";
            break;
        }
        case CREATE_TABLE_NODE:
        {
            ss << "CREATE TABLE " << create_table_sql->table_name << " (";
            for (const auto& column : create_table_sql->columns)
            {
                ss << column.col_name << " " << column.value_type << ", ";
            }
            ss << ");";
            break;
        }
        case INSERT_INTO_TABLE_NODE:
        {
            ss << "INSERT INTO " << insert_into_table_sql->table_name << " (";
            for (const auto& column : insert_into_table_sql->columns)
            {
                ss << column.col_name << ", ";
            }
            ss << ") VALUES (";
            for (const auto& value : insert_into_table_sql->values)
            {
                ss << value.ToString() << ", ";
            }
            ss << ");";
            break;
        }
        // TODO: more sql support is on designing
        default:
            ss << "Unknown node type";
            break;
        }
        return ss.str();
    }
};

}

#endif // VDBMS_SQL_PASRSER_AST_H_