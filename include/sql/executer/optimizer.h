// Copyright (c) 2024 by dingning
//
// file  : optimizer.h
// since : 2024-08-06
// desc  : This file contains all optimizer for sql's excution plan. Mainly for select sql.

#ifndef VDBMS_SQL_EXECUTER_OPTIMIZER_H_
#define VDBMS_SQL_EXECUTER_OPTIMIZER_H_

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
            vector<Column> selected_columns;
            BuildAllCols(table, selected_columns);
            SetConditionColsValueType(table, sql->compare_vector);
            response = SelectColsWithCondition(db, sql->table_name, selected_columns, result, sql->compare_vector);
        } 
        else
        {
            // check cols exist
            if (!op->CheckColsExists(table, sql->columns))
            {
                response->sql_state = FAILURE;
                response->information = "Select column not exist!";
                return response;
            }
            // check condition cols exist
            if (!CheckConditionsColsExist(table, sql->compare_vector))
            {   
                response->sql_state = FAILURE;
                response->information = "Condition column not exist!";
                return response;
            }
            // select cols
            response = SelectColsWithCondition(db, sql->table_name, sql->columns, result, sql->compare_vector);          
        }

        // after execute, delete the sql to free memory
        delete sql;

        return response;
    }

    /**
     * Get the value type of a column based on its name.
     * 
     * @param column_size The size of the column array.
     * @param col_name The name of the column to get the value type for.
     * @return The value type of the column, or RAW type if not found.
     */
    ValueType GetColumnValueType(default_amount_type column_size, Columns* columns, std::string col_name)
    {
        while (column_size >= 0)
        {
            column_size--;
            if (columns->column_name_array[column_size] == col_name)
            {
                return GetEnumType(columns->column_type_array[column_size]);
            } 
        }

        // can not found, return RAW type
        return GetEnumType(3);
    }

    bool CheckConditionsColsExist(ColumnTable* table, vector<CompareCondition>& conditions)
    {
        // check conditon cols exists
        vector<Column> cols;

        if (conditions.size() > 0)
        {
            for (auto& item : conditions)
            {
                cols.push_back(item.col);
            }
        }
        else
        {
            return true;
        }

        if (op->CheckColsExists(table, cols))
        {
            return true;
        }
        return false;
    }

    void BuildAllCols(ColumnTable* table, vector<Column>& columns)
    {
        for (default_amount_type i = 0; i < table->column_size; i++)
        {
            Column col; 
            col.col_name = table->columns.column_name_array[i];
            col.value_type = GetEnumType(table->columns.column_type_array[i]);
            columns.push_back(col);
        }
    }

    void SetConditionColsValueType(ColumnTable* table, vector<CompareCondition>& conditions)
    {
        for (auto& item: conditions)
        {
            item.col.value_type = GetColumnValueType(table->column_size, &table->columns, item.col.col_name);
        }
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
    SqlResponse* SelectCols(DB* db, string table_name, vector<Column>& columns, vector<Row*>& result)
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

        // serialize result to response
        response->sql_state = SqlState::SUCCESS;
        response->information = SerializeRowsHeader(columns);
        response->information += "\n";
        response->information += SerializeData(result);

        return response;
    }

    /**
    * Select columns from a table with conditions and return the result as a SqlResponse object.
    * @todo: Not support OR operator now!
    * @param db The database object.
    * @param table_name The name of the table to select from.
    * @param columns A vector of Column objects representing the columns to select.
    * @param result A vector of Row pointers to store the result.
    * @param conditions A vector of CompareCondition objects representing the conditions to filter the result.
    * @return A SqlResponse object containing the result of the selection.
    */
    SqlResponse* SelectColsWithCondition(DB* db, string table_name, vector<Column>& columns, vector<Row*>& result, vector<CompareCondition> conditions)
    {
        SqlResponse* response = new SqlResponse();

        ColumnTable* table;
        if (!op->CheckTableExists(db, table_name, table))
        {   
            response->sql_state = SqlState::FAILURE;
            response->information = "table " + table_name + " not exist";
            return response;
        }
        
        // set value type
        SetConditionColsValueType(table, conditions);

        std::map<string, std::vector<CompareCondition*> > conditions_map;
        std::map<string, Column*> selected_cols_map;
        default_amount_type selected_cols_amount = MergeUsedCols(conditions_map, selected_cols_map, columns, conditions);

        vector<vector<value_tag*>*> cols;
        for (default_amount_type i = 0; i < columns.size(); i++)
        {
            vector<value_tag*>* col_records = new vector<value_tag*>();

            // Load record to col_records
            CompareCondition* con = GetConditionOnCol(conditions_map, columns[i].col_name);
            if (con == nullptr)
            {
                op->FilterLoad(db, table_name, columns[i].col_name, nullptr, nullptr, *col_records);
            }
            else
            {
                Value* comp_val = new Value(con->compare_value); 
                comp_val->InitValue(columns[i].value_type);   // init a raw value
                op->FilterLoad(db, table_name, columns[i].col_name, &con->condition, comp_val, *col_records);
            }

            // add col records to cols
            cols.push_back(col_records);
        }
        
        // splice cols to row
        op->InnerJoinColumns(result, cols);

        // delete cols, values will soon be deleted after serialize to result
        for (auto col : cols) {
            delete col;
        }

        // serialize result to response
        response->sql_state = SqlState::SUCCESS;
        response->information = SerializeRowsHeader(columns);
        response->information += "\n";
        response->information += SerializeData(result, selected_cols_amount);

        return response;
    }

    default_amount_type MergeUsedCols(std::map<string, std::vector<CompareCondition*> >& conditions_map, std::map<string, Column*>& selected_cols_map, vector<Column>& columns, vector<CompareCondition>& conditions)
    { 
        InitConditionMap(conditions_map, conditions);
        InitSelectedColsMap(selected_cols_map, columns);

        default_amount_type selected_cols_amount = columns.size();

        for (auto& item : conditions)
        {
            if (!CheckColSelected(selected_cols_map, item.col.col_name))
            {
                columns.push_back(item.col);
            }
        }

        return selected_cols_amount;
    }

    void InitConditionMap(std::map<string, std::vector<CompareCondition*> >& conditions_map, vector<CompareCondition>& conditions)
    {
        for (auto& condition: conditions)
        {
            conditions_map[condition.col.col_name].push_back(&condition);
        }
    }

    CompareCondition* GetConditionOnCol(std::map<string, std::vector<CompareCondition*> >& conditions_map, string col_name)
    {
        if (conditions_map.find(col_name) == conditions_map.end())
        {
            return nullptr;
        }
        return conditions_map[col_name][0];
    }

    void InitSelectedColsMap(std::map<string, Column*>& selected_cols_map, vector<Column>& cols)
    {
        for (auto col: cols)
        {
            selected_cols_map[col.col_name] = &col;
        }
    }

    bool CheckColSelected(std::map<string, Column*>& selected_cols_map, string col_name)
    {
        if (selected_cols_map.find(col_name) == selected_cols_map.end())
        {
            return false;
        }
        return true;
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

    /**
     * Serializes the provided data into a string format.
     *
     * @param result      A vector of Row pointers containing the data to be serialized.
     * @param amount      The amount type to be used for serialization.
     * @param tag_open    An optional flag indicating whether to include tags in the serialized data. Defaults to false.
     *
     * @return A string representation of the serialized data.
     */
    string SerializeData(vector<Row*>& result, default_amount_type amount, bool tag_open = false)
    {
        string records_data;

        if (tag_open)
        {
            for (auto& item : result)
            {
                records_data += item->ToStringWithTag(amount);
                records_data += "\n";
            }
        }
        else
        {
            for (auto& item : result)
            {
                records_data += item->ToString(amount);
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

#endif // VDBMS_SQL_EXECUTER_OPTIMIZER_H_