// Copyright (c) 2024 by dingning
//
// file  : column_table.h
// since : 2024-07-17
// desc  : Organize data by column, not by row.
/*
table_name_size.                    such as 5
table_name.                         such as test/0
column_size.                        such as 1
column_name_array.                  such as |column1|
column_type_array.                  such as |0(VECTOR)|
column_index_type_array.            such as |NONE|
column_storage_address.             such as |0x0000|

*/


#ifndef VDBMS_FLAT_INDEX_
#define VDBMS_FLAT_INDEX_

#include <string>

#include "../../config.h" 

namespace tiny_v_dbms {

using std::string; 

struct Column_Table 
{

    // information about the table
    string table_name;
    string table_type;
    default_amount_type column_size;                // amount of column
    string* column_name_array;                      // names of each column
    default_enum_type* column_type_array;           // type of each column
    default_enum_type* column_index_type_array;     // index type of each column

    // information about where to store actual data
    default_address_type* column_storage_address;   // where to store each column, is the address of first block

    /**
    * @brief return the space cost of this table header.
    */
    default_length_size GetSize() {
        return sizeof(string) * 2 + sizeof(default_amount_type) + (sizeof(string) + sizeof(default_enum_type) * 2 + sizeof(default_address_type)) * column_size;
    }

    /**
     * @brief Serialize the Column_Table struct to a binary buffer.
     * 
     * @return A pointer to the serialized data.
     * @return The length of the serialized data.
     */
    std::pair<char*, size_t> Serialize() {
        size_t total_size = sizeof(string) * 2 + sizeof(default_amount_type) + (sizeof(string) + sizeof(default_enum_type) * 2 + sizeof(default_address_type)) * column_size;
        char* buffer = new char[total_size];

        size_t offset = 0;

        // Write the table name
        size_t table_name_size = table_name.size();
        memcpy(buffer + offset, &table_name_size, sizeof(size_t));
        offset += sizeof(size_t);
        memcpy(buffer + offset, table_name.c_str(), table_name_size);
        offset += table_name_size;

        // Write the table type
        size_t table_type_size = table_type.size();
        memcpy(buffer + offset, &table_type_size, sizeof(size_t));
        offset += sizeof(size_t);
        memcpy(buffer + offset, table_type.c_str(), table_type_size);
        offset += table_type_size;

        // Write the column size
        memcpy(buffer + offset, &column_size, sizeof(default_amount_type));
        offset += sizeof(default_amount_type);

        // Write the column name array
        memcpy(buffer + offset, &column_size, sizeof(default_amount_type));
        offset += sizeof(default_amount_type);
        for (default_amount_type i = 0; i < column_size; ++i) {
            size_t column_name_size = column_name_array[i].size();
            memcpy(buffer + offset, &column_name_size, sizeof(size_t));
            offset += sizeof(size_t);
            memcpy(buffer + offset, column_name_array[i].c_str(), column_name_size);
            offset += column_name_size;
        }

        // Write the column type array
        memcpy(buffer + offset, &column_size, sizeof(default_amount_type));
        offset += sizeof(default_amount_type);
        for (default_amount_type i = 0; i < column_size; ++i) {
            memcpy(buffer + offset, &column_type_array[i], sizeof(default_enum_type));
            offset += sizeof(default_enum_type);
        }

        // Write the column index type array
        memcpy(buffer + offset, &column_size, sizeof(default_amount_type));
        offset += sizeof(default_amount_type);
        for (default_amount_type i = 0; i < column_size; ++i) {
            memcpy(buffer + offset, &column_index_type_array[i], sizeof(default_enum_type));
            offset += sizeof(default_enum_type);
        }

        // Write the column storage address array
        memcpy(buffer + offset, &column_size, sizeof(default_amount_type));
        offset += sizeof(default_amount_type);
        for (default_amount_type i = 0; i < column_size; ++i) {
            memcpy(buffer + offset, &column_storage_address[i], sizeof(default_address_type));
            offset += sizeof(default_address_type);
        }

        return std::make_pair(buffer, total_size);
    }

    /**
     * @brief Deserialize a Column_Table struct from a binary buffer.
     * 
     * @param buffer The binary buffer to read from.
     * @param length The length of the binary buffer.
     */
    void Deserialize(const char* buffer, size_t length) {
        size_t offset = 0;

        // Read the table name
        size_t table_name_size;
        memcpy(&table_name_size, buffer + offset, sizeof(size_t));
        offset += sizeof(size_t);
        table_name.resize(table_name_size);
        memcpy(&table_name[0], buffer + offset, table_name_size);
        offset += table_name_size;

        // Read the table type
        size_t table_type_size;
        memcpy(&table_type_size, buffer + offset, sizeof(size_t));
        offset += sizeof(size_t);
        table_type.resize(table_type_size);
        memcpy(&table_type[0], buffer + offset, table_type_size);
        offset += table_type_size;

        // Read the column size
        memcpy(&column_size, buffer + offset, sizeof(default_amount_type));
        offset += sizeof(default_amount_type);

        // Read the column name array
        column_name_array = new string[column_size];
        for (default_amount_type i = 0; i < column_size; ++i) {
            size_t column_name_size;
            memcpy(&column_name_size, buffer + offset, sizeof(size_t));
            offset += sizeof(size_t);
            column_name_array[i].resize(column_name_size);
            memcpy(&column_name_array[i][0], buffer + offset, column_name_size);
            offset += column_name_size;
        }

        // Read the column type array
        column_type_array = new default_enum_type[column_size];
        for (default_amount_type i = 0; i < column_size; ++i) {
            memcpy(&column_type_array[i], buffer + offset, sizeof(default_enum_type));
            offset += sizeof(default_enum_type);
        }

        // Read the column index type array
        column_index_type_array = new default_enum_type[column_size];
        for (default_amount_type i = 0; i < column_size; ++i) {
            memcpy(&column_index_type_array[i], buffer + offset, sizeof(default_enum_type));
            offset += sizeof(default_enum_type);
        }

        // Read the column storage address array
        column_storage_address = new default_address_type[column_size];
        for (default_amount_type i = 0; i < column_size; ++i) {
            memcpy(&column_storage_address[i], buffer + offset, sizeof(default_address_type));
            offset += sizeof(default_address_type);
        }
    }
};

}

#endif // VDBMS_FLAT_INDEX_