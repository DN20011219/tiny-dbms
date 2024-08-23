// Copyright (c) 2024 by dingning
//
// file  : column_table.h
// since : 2024-07-17
// desc  : Organize data by column, not by row.
/*
table_name_size.                    such as |5|
table_name.                         such as |test/0|
column_size.                        such as |1|
column_name_array.                  such as |column1|
column_type_array.                  such as |0(VECTOR)|
column_length_array                 such as |20|
column_index_type_array.            such as |NONE|
column_storage_address_array.             such as |0x0000|

*/


#ifndef VDBMS_META_TABLE_COLUMN_TABLE_H_
#define VDBMS_META_TABLE_COLUMN_TABLE_H_

#include <iostream>
#include <string>

#include "../../config.h" 
#include "../column/column_array.h"

namespace tiny_v_dbms {

using std::string; 

class ColumnTable 
{
public:

    // information about the table
    string table_name;
    default_enum_type table_type;
    default_amount_type column_size;                      // amount of column

    // struct Columns {
    //     string* column_name_array;                            // names of each column
    //     default_enum_type* column_type_array;                 // type of each column
    //     default_length_size* column_length_array;                   // space cost of each column
    //     default_enum_type* column_index_type_array;           // index type of each column
    //     default_address_type* column_storage_address_array;   // where to store each column, is the address of first block number in table data file
    // } columns;

    Columns columns;

    ColumnTable() : column_size(0), columns() {}

    ColumnTable(default_amount_type col_num) : column_size(col_num) {
        if (col_num > 0) {
            columns.column_name_array = new string[col_num];
            columns.column_type_array = new default_enum_type[col_num];
            columns.column_length_array = new default_length_size[col_num];
            columns.column_index_type_array = new default_enum_type[col_num];
            columns.column_storage_address_array = new default_address_type[col_num];
        }
    }

    ColumnTable(const ColumnTable& table) {
        table_name = table.table_name;
        table_type = table.table_type;
        column_size = table.column_size;
        columns.column_name_array = new string[column_size];
        columns.column_type_array = new default_enum_type[column_size];
        columns.column_length_array = new default_length_size[column_size];
        columns.column_index_type_array = new default_enum_type[column_size];
        columns.column_storage_address_array = new default_address_type[column_size];
        for (default_length_size i = 0; i < column_size; ++i) {
            columns.column_name_array[i] = table.columns.column_name_array[i];
            columns.column_type_array[i] = table.columns.column_type_array[i];
            columns.column_length_array[i] = table.columns.column_length_array[i];
            columns.column_index_type_array[i] = table.columns.column_index_type_array[i];
            columns.column_storage_address_array[i] = table.columns.column_storage_address_array[i];
        }
    }

    ColumnTable(ColumnTable&& other) 
        : table_name(other.table_name), 
        table_type(other.table_type), 
        column_size(other.column_size), 
        columns(other.columns) {
        other.column_size = 0; // mark the other object as empty
    }

    ColumnTable& operator=(const ColumnTable& other) {
        if (this != &other) {
            if (column_size != 0) {
                delete[] columns.column_name_array;
                delete[] columns.column_type_array;
                delete[] columns.column_length_array;
                delete[] columns.column_index_type_array;
                delete[] columns.column_storage_address_array;
            }
            table_name = other.table_name;
            table_type = other.table_type;
            column_size = other.column_size;
            columns.column_name_array = new string[column_size];
            columns.column_type_array = new default_enum_type[column_size];
            columns.column_length_array = new default_length_size[column_size];
            columns.column_index_type_array = new default_enum_type[column_size];
            columns.column_storage_address_array = new default_address_type[column_size];
            for (default_length_size i = 0; i < column_size; ++i) {
                columns.column_name_array[i] = other.columns.column_name_array[i];
                columns.column_type_array[i] = other.columns.column_type_array[i];
                columns.column_length_array[i] = other.columns.column_length_array[i];
                columns.column_index_type_array[i] = other.columns.column_index_type_array[i];
                columns.column_storage_address_array[i] = other.columns.column_storage_address_array[i];
            }
        }
        return *this;
    }

    ColumnTable& operator=(ColumnTable&& other) {
        if (this != &other) {
            if (column_size != 0) {
                delete[] columns.column_name_array;
                delete[] columns.column_type_array;
                delete[] columns.column_length_array;
                delete[] columns.column_index_type_array;
                delete[] columns.column_storage_address_array;
            }
            table_name = other.table_name;
            table_type = other.table_type;
            column_size = other.column_size;
            columns = other.columns;
            other.column_size = 0; // mark the other object as empty
        }
        return *this;
    }

    /**
     * Inserts a new column into the table.
     * 
     * This function increases the column size by 1 and updates the column name array.
     * 
     * @param column_name The name of the new column to be inserted.
     * @param column_type The data type of the new column (e.g. integer, string, etc.).
     * @param column_index_type The type of index to be created for the new column (e.g. primary key, unique, etc.).
     * @param column_storage_address The storage address for the new column.
     * 
     * Example:
     * ```cpp
     * InsertColumn("Age", INTEGER, PRIMARY_KEY, 0x1000);
     * ```
     * This example inserts a new column named "Age" with an integer data type, a primary key index, and a storage address of 0x1000.
    */
    void InsertColumn(
        string column_name, 
        default_enum_type column_type,
        default_length_size column_length,
        default_enum_type column_index_type,
        default_address_type column_storage_address
        )
    {

        if (column_size == 0)
        {   
            column_size++;

            columns.column_name_array = new string[1];
            columns.column_name_array[0] = column_name;

            columns.column_type_array = new default_enum_type[1];
            columns.column_type_array[0] = column_type;

            columns.column_length_array = new default_length_size[1];
            columns.column_length_array[0] = column_length;

            columns.column_index_type_array = new default_enum_type[1];
            columns.column_index_type_array[0] = column_index_type; 

            columns.column_storage_address_array = new default_address_type[1];
            columns.column_storage_address_array[0] = column_storage_address;

            return;
        }

        column_size++;

        // Update the column name array
        string* cache_column_name_array = new string[column_size];
        for (int i = 0; i < column_size - 1; i++)
        {
            cache_column_name_array[i] = columns.column_name_array[i];
        }
        cache_column_name_array[column_size - 1] = column_name;

        delete[] columns.column_name_array;
        columns.column_name_array = cache_column_name_array;

        // Update the column type array
        default_enum_type* cache_column_type_array = new default_enum_type[column_size];
        for (int i = 0; i < column_size - 1; i++)
        {
            cache_column_type_array[i] = columns.column_type_array[i];
        }
        cache_column_type_array[column_size - 1] = column_type;

        delete[] columns.column_type_array;
        columns.column_type_array = cache_column_type_array;

        // update column length array
        default_length_size* cache_column_length_array = new default_length_size[column_size];
        for (int i = 0; i < column_size - 1; i++)
        {
            cache_column_length_array[i] = columns.column_length_array[i];
        }
        cache_column_length_array[column_size - 1] = column_length;

        delete[] columns.column_length_array;
        columns.column_length_array = cache_column_length_array;
    
        // Update the column index type array
        default_enum_type* cache_column_index_type_array = new default_enum_type[column_size];
        for (int i = 0; i < column_size - 1; i++)
        {
            cache_column_index_type_array[i] = columns.column_index_type_array[i];
        }
        cache_column_index_type_array[column_size - 1] = column_index_type;

        delete[] columns.column_index_type_array;
        columns.column_index_type_array = cache_column_index_type_array;

        // Update the column storage address array
        default_address_type* cache_column_storage_address_array = new default_address_type[column_size];
        for (int i = 0; i < column_size - 1; i++)
        {
            cache_column_storage_address_array[i] = columns.column_storage_address_array[i];
        }
        cache_column_storage_address_array[column_size - 1] = column_storage_address;

        delete[] columns.column_storage_address_array;
        columns.column_storage_address_array = cache_column_storage_address_array;
   
    }   

    /**
     * @brief Calculates the total length of the serialized data for the ColumnTable struct.
     * 
     * This function calculates the total length of the serialized data, which includes:
     * - The size of the table name (sizeof(default_length_size) + table_name.size())
     * - The size of the table type (sizeof(table_type))
     * - The size of the column amount (sizeof(default_amount_type))
     * - The size of the column name array (sizeof(default_length_size) + columns.column_name_array[i].size() for each column)
     * - The size of the column type array (column_size * sizeof(default_enum_type))
     * - The size of the column length array (column_size * sizeof(default_length_size))
     * - The size of the column index type array (column_size * sizeof(default_enum_type))
     * - The size of the column storage address array (column_size * sizeof(default_address_type))
     * 
     * @return The total length of the serialized data.
     */
    default_length_size GetLength() {
        default_length_size length = 0;

        // table name size + table name
        length += sizeof(default_length_size) + table_name.size();

        // table type
        length += sizeof(table_type);

        // column amount
        length += sizeof(default_amount_type);

        // column name array
        for (default_amount_type i = 0; i < column_size; ++i) {
            length += sizeof(default_length_size) + columns.column_name_array[i].size();
        }

        // column type array
        length += column_size * sizeof(default_enum_type);

        // column length array
        length += column_size * sizeof(default_length_size);

        // column index type array
        length += column_size * sizeof(default_enum_type);

        // column storage address array
        length += column_size * sizeof(default_address_type);

        return length;
    }

    /**
     * @brief Serialize the ColumnTable struct to a binary buffer.
     * 
     * This function serializes the ColumnTable struct into a binary buffer, which includes:
     * - The table name
     * - The table type
     * - The column amount
     * - The column name array
     * - The column type array
     * - The column length array
     * - The column index type array
     * - The column storage address array
     * 
     * @param buffer The binary buffer to serialize into.
     * @param begin_offset The starting offset in the buffer to begin serialization.
     * 
     * @return The length of the serialized data.
     */
    default_length_size Serialize(char* buffer, default_length_size begin_offset) {
        default_length_size offset = 0;

        // Write the table name
        default_length_size table_name_size = table_name.size();
        memcpy(buffer + offset + begin_offset, &table_name_size, sizeof(default_length_size));
        offset += sizeof(default_length_size);
        memcpy(buffer + offset + begin_offset, table_name.c_str(), table_name_size);
        offset += table_name_size;

        // Write the table type
        default_length_size table_type_size = sizeof(table_type);
        memcpy(buffer + offset + begin_offset, &table_type, table_type_size);
        offset += table_type_size;

        // Write the column amount
        memcpy(buffer + offset + begin_offset, &column_size, sizeof(default_amount_type));
        offset += sizeof(default_amount_type);

        // Write the column name array
        for (default_amount_type i = 0; i < column_size; ++i) {
            default_length_size column_name_size = columns.column_name_array[i].size();
            memcpy(buffer + offset + begin_offset, &column_name_size, sizeof(default_length_size));
            offset += sizeof(default_length_size);
            memcpy(buffer + offset + begin_offset, columns.column_name_array[i].c_str(), column_name_size);
            offset += column_name_size;
        }

        // Write the column type array
        for (default_amount_type i = 0; i < column_size; ++i) {
            memcpy(buffer + offset + begin_offset, &columns.column_type_array[i], sizeof(default_enum_type));
            offset += sizeof(default_enum_type);
        }

        // Write the column length array
        for (default_amount_type i = 0; i < column_size; ++i) {
            memcpy(buffer + offset + begin_offset, &columns.column_length_array[i], sizeof(default_length_size));
            offset += sizeof(default_enum_type);
        }

        // Write the column index type array
        for (default_amount_type i = 0; i < column_size; ++i) {
            memcpy(buffer + offset + begin_offset, &columns.column_index_type_array[i], sizeof(default_enum_type));
            offset += sizeof(default_enum_type);
        }

        // Write the column storage address array
        for (default_amount_type i = 0; i < column_size; ++i) {
            memcpy(buffer + offset + begin_offset, &columns.column_storage_address_array[i], sizeof(default_address_type));
            offset += sizeof(default_address_type);
        }

        return offset;
    }

    /**
     * @brief Deserialize a ColumnTable struct from a binary buffer.
     * 
     * @param buffer The binary buffer to read from.
     * @param read_offset Start read offset.
     */
    void Deserialize(const char* buffer, default_length_size read_offset) {
        default_length_size offset = read_offset;

        // Read the table name
        default_length_size table_name_size;
        memcpy(&table_name_size, buffer + offset, sizeof(default_length_size));
        // std::cout << "table_name_size" << table_name_size << std::endl;
        offset += sizeof(default_length_size);
        table_name.resize(table_name_size);
        memcpy(&table_name[0], buffer + offset, table_name_size);
        offset += table_name_size;
        // std::cout << "table_name_size" << table_name << std::endl;

        // Read the table type
        memcpy(&table_type, buffer + offset, sizeof(default_enum_type));
        offset += sizeof(default_enum_type);

        // Read the column size
        memcpy(&column_size, buffer + offset, sizeof(default_amount_type));
        // std::cout << "column_size" << column_size << std::endl;
        offset += sizeof(default_amount_type);

        // Read the column name array
        columns.column_name_array = new string[column_size];
        for (default_amount_type i = 0; i < column_size; ++i) {
            default_length_size column_name_size;
            memcpy(&column_name_size, buffer + offset, sizeof(default_length_size));
            offset += sizeof(default_length_size);
            columns.column_name_array[i].resize(column_name_size);
            memcpy(&columns.column_name_array[i][0], buffer + offset, column_name_size);
            offset += column_name_size;
        }

        // Read the column type array
        columns.column_type_array = new default_enum_type[column_size];
        for (default_amount_type i = 0; i < column_size; ++i) {
            memcpy(&columns.column_type_array[i], buffer + offset, sizeof(default_enum_type));
            offset += sizeof(default_enum_type);
        }

        // Read the column length array
        columns.column_length_array = new default_length_size[column_size];
        for (default_amount_type i = 0; i < column_size; ++i) {
            memcpy(&columns.column_length_array[i], buffer + offset, sizeof(default_length_size));
            offset += sizeof(default_length_size);
        }

        // Read the column index type array
        columns.column_index_type_array = new default_enum_type[column_size];
        for (default_amount_type i = 0; i < column_size; ++i) {
            memcpy(&columns.column_index_type_array[i], buffer + offset, sizeof(default_enum_type));
            offset += sizeof(default_enum_type);
        }

        // Read the column storage address array
        columns.column_storage_address_array = new default_address_type[column_size];
        for (default_amount_type i = 0; i < column_size; ++i) {
            memcpy(&columns.column_storage_address_array[i], buffer + offset, sizeof(default_address_type));
            offset += sizeof(default_address_type);
        }
    }
};

}

#endif // VDBMS_META_TABLE_COLUMN_TABLE_H_