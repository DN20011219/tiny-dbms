// Copyright (c) 2024 by dingning
//
// file  : optimizer.h
// since : 2024-08-06
// desc  : This file contains all optimizer for sql's excution plan. Mainly for select sql.

#ifndef VDBMS_SQL_EXECUTER_PLAN_H_
#define VDBMS_SQL_EXECUTER_PLAN_H_

#include "../parser/ast.h"
#include "operator.h"

namespace tiny_v_dbms {

struct ProjectionOp
{
    string table_name;
    vector<Column*> cols;


};

struct Condition
{
    Column* col;
    Comparator comparator;
    Value* compare_val;
};


struct OperationPlan
{

};

class Optimizer
{
private:
    Operator* op;

public:

    Optimizer(Operator* op) : op(op)
    {

    }

    /**
     * ExecuteSelect function to execute a SELECT statement on a single table in the database.
     * @todo now have not support condition select!
     * @param db: a pointer to the database object
     * @param sql: a pointer to the SelectFromOneTableSql object containing the SELECT statement
     * @return a pointer to a SqlResponse object containing the result of the query
     */
    SqlResponse* ExecuteSelect(DB* db, SelectFromOneTableSql* sql)
    {
        SqlResponse* response;

        ColumnTable* table;
        if (!op->GetTable(*db, sql->table_name, table))
        {   
            response = new SqlResponse();
            response->sql_state = FAILURE;
            response->information = "Db don't have table named " + sql->table_name;
            return response;
        }
        
        // execute select
        vector<Row*> result;
        // select all
        if (sql->columns.size() == 1 && sql->columns[0].col_name == "*")
        {
            response = SelectAll(db, sql->table_name, result);
        }
        else
        {
            // need additional check
            if (!op->CheckColsExists(table, sql->columns))
            {
                response->sql_state = FAILURE;
                response->information = "Select column not exist!";
                return response;
            }
            
            // select some cols
            response = SelectCols(db, sql->table_name, sql->columns, result);          
        }

        // after execute, delete the sql to free memory
        delete sql;

        return response;
    }

    /**
     * SelectAll function to retrieve all records from a specified table in a database.
     * 
     * @param db: a pointer to the database object
     * @param table_name: the name of the table to retrieve records from
     * @param result: a vector of Row pointers to store the retrieved records
     * @return a pointer to a SqlResponse object indicating the result of the operation
     */
    SqlResponse* SelectAll(DB* db, string table_name, vector<Row*>& result)
    {
        SqlResponse* response = new SqlResponse();

        ColumnTable* table;
        if (!op->CheckTableExists(db, table_name, table))
        {   
            response->sql_state = SqlState::FAILURE;
            response->information = "table " + table_name + " not exist";
            return response;
        }
        
        vector<vector<value_tag*>*> cols;

        for (default_amount_type i = 0; i < table->column_size; i++)
        {
            vector<value_tag*>* col_records = new vector<value_tag*>();

            // Load record to col_records
            op->FilterLoad(db, table_name, table->columns.column_name_array[i], nullptr, nullptr, *col_records);

            // add col records to cols
            cols.push_back(col_records);
        }
        
        // splice cols to row
        op->InnerJoinColumns(result, cols);

        // delete cols, values will be delete after serialize to result
        for (auto col : cols) {
            delete col;
        }

        // serialize result to response, and delete the values after user
        response->sql_state = SqlState::SUCCESS;
        response->information = SerializeRowsHeader(table);
        response->information += "\n";
        response->information += SerializeData(result);

        // FreeMemory(result); no need to free, value_tag store the value object in stack
        
        return response;
    }

    /**
     * SelectCols function to retrieve specific columns from a specified table in a database.
     * 
     * @param db: a pointer to the database object
     * @param table_name: the name of the table to retrieve records from
     * @param columns: a vector of Column objects specifying the columns to retrieve
     * @param result: a vector of Row pointers to store the retrieved records
     * @return a pointer to a SqlResponse object indicating the result of the operation
     */
    SqlResponse* SelectCols(DB* db, string table_name, vector<Column>& columns , vector<Row*>& result)
    {
        SqlResponse* response = new SqlResponse();

        ColumnTable* table;
        if (!op->CheckTableExists(db, table_name, table))
        {   
            response->sql_state = SqlState::FAILURE;
            response->information = "table " + table_name + " not exist";
            return response;
        }
        
        vector<vector<value_tag*>*> cols;

        for (default_amount_type i = 0; i < columns.size(); i++)
        {
            vector<value_tag*>* col_records = new vector<value_tag*>();

            // Load record to col_records
            op->FilterLoad(db, table_name, columns[i].col_name, nullptr, nullptr, *col_records);

            // add col records to cols
            cols.push_back(col_records);
        }
        
        // splice cols to row
        op->InnerJoinColumns(result, cols);

        // delete cols, values will soon be deleted after serialize to result
        for (auto col : cols) {
            delete col;
        }

        // serialize result to response, and delete the values after user
        response->sql_state = SqlState::SUCCESS;
        response->information = SerializeRowsHeader(columns);
        response->information += "\n";
        response->information += SerializeData(result);

        // FreeMemory(result);

        return response;
    }

    /**
     * SerializeRowsHeader function to generate a string representation of the column headers for a table.
     * 
     * @param table: a pointer to a ColumnTable object containing the table metadata
     * @return a string representing the column headers, separated by pipes (|)
     */
    string SerializeRowsHeader(ColumnTable* table)
    {
        string header_str;
        header_str += "| ";
        for (default_amount_type i = 0; i < table->column_size; i++)
        {
            header_str += table->columns.column_name_array[i];
            header_str += " | ";
        }
        return header_str;
    }

    /**
     * SerializeRowsHeader function to generate a string representation of the column headers for a set of columns.
     * 
     * @param columns: a vector of Column objects specifying the columns to include in the header
     * @return a string representing the column headers, separated by pipes (|)
     */
    string SerializeRowsHeader(vector<Column>& columns)
    {
        string header_str;
        header_str += "| ";
        for (default_amount_type i = 0; i < columns.size(); i++)
        {
            header_str += columns[i].col_name;
            header_str += " | ";
        }
        return header_str;
    }

    /**
     * SerializeData function to generate a string representation of the data in a vector of Row objects.
     * 
     * @param result: a vector of Row pointers containing the data to be serialized
     * @param tag_open: an optional boolean parameter indicating whether to include tags in the serialized data (default: false)
     * @return a string representing the serialized data
     */
    string SerializeData(vector<Row*>& result, bool tag_open = false)
    {
        string records_data;

        if (tag_open)
        {
            for (auto& item : result)
            {
                records_data += item->ToStringWithTag();
                records_data += "\n";
            }
        } 
        else
        {
            for (auto& item : result)
            {
                records_data += item->ToString();
                records_data += "\n";
            }
        }

        return records_data;
    }

    // void FreeMemory(vector<Row*>& result)
    // {
    //     for (auto& item : result)
    //     {
    //         delete item;
    //     }
    // }

};

}

#endif // VDBMS_SQL_EXECUTER_PLAN_H_