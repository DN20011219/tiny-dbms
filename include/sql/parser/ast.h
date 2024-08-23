// Copyright (c) 2024 by dingning
//
// file  : condition_node.h
// since : 2024-07-26
// desc  : this is a Abstract Syntax Tree(AST) node, means here are sqls to execute.

#ifndef VDBMS_SQL_PASRSER_AST_H_
#define VDBMS_SQL_PASRSER_AST_H_

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <sstream>

#include "../../meta/value.h"
#include "../../config.h"
#include "../sql_struct.h"

#include "token.h"

using std::vector;
using std::map;
using std::string;
using std::stringstream;


namespace tiny_v_dbms {

class CreateDatabaseSql
{
public: 
    string db_name;

public:
    // extract information from tokens
    CreateDatabaseSql(vector<Token> tokens)
    {   
        // CREATE DATABASE db_name ;
        db_name = tokens[2].value;
    }
};
class CreateTableSql
{
public:
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
class InsertIntoTableSql
{
public:
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
class SelectFromOneTableSql
{
public:
    string table_name;
    vector<Column> columns;
    // map<string, Column*> column_map;
    vector<CompareCondition> compare_vector;
    vector<Operation> operation_vector;

public:
    // extract information from tokens
    SelectFromOneTableSql(vector<Token> tokens)
    {
        // SELECT * FROM table1;
        // SELECT a, b FROM table1 where c > 0 AND d = 1;

        // set select col, begin from offset 1
        int token_flag = 1;
        bool pre_is_asterisk = false;
        while(token_flag < tokens.size())
        {
            // if find FROM, then break
            if (tokens[token_flag].type == TokenType::KEYWORD_T && tokens[token_flag].value == "FROM")
            {
                break;
            }

            // if pre is *, and still have col, this sql is wrong!
            if (pre_is_asterisk)
            {
                throw std::runtime_error("Sql wrong, * can not be used with other column name");
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

            // if this col is *, set pre_is_asterisk is true, means follows must be FROM,
            if (tokens[token_flag].value == "*")
            {
                pre_is_asterisk = true;
            }
        }

        // skip FROM
        token_flag++;

        // set table name
        table_name = tokens[token_flag].value;
        token_flag++;

        // try get where condition
        CompareCondition* pre_compare = nullptr;
        Operation* pre_op = nullptr;
        while(token_flag < tokens.size())
        {
            // if find ;, then break
            if (tokens[token_flag].type == TokenType::OPERATOR_T && tokens[token_flag].value == ";")
            {
                break;
            }

            // try match 4/5 tokens one time: "AND"/"OR" col_name comparator value
            if (tokens[token_flag].type == TokenType::KEYWORD_T && (tokens[token_flag].value == "AND" || tokens[token_flag].value == "OR"))
            {
                // pre_compare == nullptr, means use AND/ OR but there has no pre compare, this is not allowed
                if (pre_compare == nullptr && pre_op == nullptr)
                {
                    throw std::runtime_error("Sql wrong, use AND/OR but has no pre condition");
                }

                // try match another 3 tokens
                if (
                    tokens[token_flag + 1].type == IDENTIFIER_T 
                    && tokens[token_flag + 2].type == OPERATOR_T 
                    && (tokens[token_flag + 3].type == NUMBER_T || tokens[token_flag + 3].type == STRING_T)
                    ) 
                {
                    // buil CompareCondition
                    CompareCondition new_compare;
                    Column compare_column; compare_column.col_name = tokens[token_flag + 1].value;
                    new_compare.col = std::move(compare_column);
                    new_compare.condition = SwitchComparator(tokens[token_flag + 2].value);
                    new_compare.compare_value = tokens[token_flag + 3].value;
                    // store CompareCondition
                    compare_vector.push_back(std::move(new_compare));

                    // buil Operation
                    Operation op;
                    if (pre_compare == nullptr) 
                    {
                        op.left_op = std::move(pre_op);
                    }
                    else 
                    {
                        op.left_leaf = std::move(*pre_compare);
                        pre_compare = nullptr;
                    }
                    op.opreator = SwitchOperator(tokens[token_flag].value);
                    op.right_leaf = std::move(new_compare);

                    // cache op to pre_op
                    pre_op = &op;
                    // store Operation
                    operation_vector.push_back(std::move(op));

                    token_flag += 4;
                    continue;
                } 
                
                // try match another 4 tokens, != <> maybe need 2 token
                if (                    
                    tokens[token_flag + 1].type == IDENTIFIER_T 
                    && tokens[token_flag + 2].type == OPERATOR_T 
                    && tokens[token_flag + 3].type == OPERATOR_T 
                    && (tokens[token_flag + 4].type == NUMBER_T || tokens[token_flag + 4].type == STRING_T))
                {
                    
                    // buil CompareCondition
                    CompareCondition new_compare;
                    Column compare_column; compare_column.col_name = tokens[token_flag + 1].value;
                    new_compare.col = std::move(compare_column);
                    new_compare.condition = SwitchComparator(tokens[token_flag + 2].value + tokens[token_flag + 3].value);
                    new_compare.compare_value = tokens[token_flag + 3].value;
                    // store CompareCondition
                    compare_vector.push_back(std::move(new_compare));

                    // buil Operation
                    Operation op;
                    if (pre_compare == nullptr) 
                    {
                        op.left_op = std::move(pre_op);
                    }
                    else 
                    {
                        op.left_leaf = std::move(*pre_compare);
                        pre_compare = nullptr;
                    }
                    op.opreator = SwitchOperator(tokens[token_flag].value);
                    op.right_leaf = std::move(new_compare);

                    // cache op to pre_op
                    pre_op = &op;
                    // store Operation
                    operation_vector.push_back(std::move(op));

                    token_flag += 5;
                    continue;
                }

                // can not match any compare, throw error
                throw std::runtime_error("Sql wrong, condition can not be parse");
            }

            // try match 4 tokens one time: WHERE col_name comparator value
            if (
                tokens[token_flag].value == "WHERE"
                && tokens[token_flag + 1].type == IDENTIFIER_T 
                && tokens[token_flag + 2].type == OPERATOR_T 
                && (tokens[token_flag + 3].type == NUMBER_T || tokens[token_flag + 3].type == STRING_T)
                ) 
            {
                // build CompareCondition
                CompareCondition new_compare;
                Column compare_column; compare_column.col_name = tokens[token_flag + 1].value;
                new_compare.col = std::move(compare_column);
                new_compare.condition = SwitchComparator(tokens[token_flag + 2].value);
                new_compare.compare_value = tokens[token_flag + 3].value;

                // store CompareCondition
                compare_vector.push_back(std::move(new_compare));
                // cache new_compare to pre_compare 
                pre_compare = &new_compare;

                token_flag += 4;
                continue;
            }

            // can not match any type of condition, break
            break;
        }

        // check sql has been parsed end
        if (!(token_flag == tokens.size() - 1 && tokens[token_flag].type == TokenType::OPERATOR_T && tokens[token_flag].value == ";"))
        {
            throw std::runtime_error("Sql wrong, not end with ; or can not be parsed");
        }   
    }
};
// TODO:Select using join
// struct SelectFromTableWithJoinSql
// {
//     string table_name;
//     vector<Column> columns;
// };
struct DropDatabaseSql
{
public:
    string db_name;

public:
    DropDatabaseSql(vector<Token> tokens);
};
struct DropTableSql
{
public:
    string table_name;

public:
    DropTableSql(vector<Token> tokens);
};

class AST
{
private:
    NodeType node_type;

public:

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
        case SELECT_FROM_ONE_TABLE_NODE:
            delete select_from_one_table_sql;
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
            case NodeType::SELECT_FROM_ONE_TABLE_NODE:
                select_from_one_table_sql = static_cast<SelectFromOneTableSql*>(data);
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
            case SELECT_FROM_ONE_TABLE_NODE:
            {
                ss << "SELECT ";
                for (const auto& column : select_from_one_table_sql->columns) {
                    ss << column.col_name << ", ";
                }
                ss << " FROM " << select_from_one_table_sql->table_name;
                if (!select_from_one_table_sql->compare_vector.empty()) {
                    ss << " WHERE ";
                    for (const auto& condition : select_from_one_table_sql->compare_vector) {
                        ss << condition.col.col_name << " " << condition.condition << " " << condition.compare_value << " ";
                    }
                }
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