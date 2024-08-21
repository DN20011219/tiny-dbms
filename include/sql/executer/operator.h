// Copyright (c) 2024 by dingning
//
// file  : operator.h
// since : 2024-07-19
// desc  : this file contains all operator to execute one sql.


#ifndef VDBMS_SQL_EXECUTER_OPERATOR_H_
#define VDBMS_SQL_EXECUTER_OPERATOR_H_

#include <iostream>

#include "../../config.h"
// meta struct
#include "../../meta/db/db.h"
#include "../../meta/value.h"
#include "../../meta/value_tag.h"
// sql
#include "../../sql/sql_struct.h"
// memory buffer poll
#include "../../storage/file_management.h"
#include "../../storage/block_file_management.h"
#include "../../storage/memory/lock_watcher.h"
// table header & table data
#include "../../meta/table/column_table.h"
#include "../../meta/block/table_block.h"
#include "../../meta/block/data_block.h"

// log
#include "../../memory/log_central_management.h"
#include "../../meta/log.h"
// util
#include "../../utils/cal_file_url_util.h"


namespace tiny_v_dbms {


class ColumnScan
{
    /**
     * @param table_name
     * @param column_name
     * @param criterion
     * @brief return record meet the criterion
     */
};

class ColumnComplete
{
    /**
     * @param one_column_data
     * @param to_compelete_column_names
     * @brief Concatenate data from different columns(one_column_data, to_compelete_columns) into a single result, other col's record need extra operate
     */
};

class ColumnInnerJoin
{
    /**
     * @param firsrt_column_data
     * @param second_column_data
     * 
     * @brief return records exists on both of columns
     */
};


class Operator {
private:
    FileManagement* file_mm;
    BlockFileManagement* bfmm;
    LockWatcher* lw;

    // get install path from file
    void GetInstallPath(string& install_path) 
    {
        FileManagement file_mm;
        string cache_file_uri = INSTALL_PATH_CACHE_FILE;
        std::ifstream file_read = file_mm.ReadOrCreateFile(cache_file_uri);
        if (!file_read) {
            throw std::runtime_error("Failed to open install file for reading");
        }
        getline(file_read, install_path);
        file_read.close();

    }

    void WriteInstallPath(string& install_path) 
    {
        string cache_file_uri = INSTALL_PATH_CACHE_FILE;
        std::ofstream file_write;
        file_write.open(cache_file_uri);
        file_write.write(install_path.c_str(), install_path.length());
        file_write.close();
    }

    bool InstallDBMS() 
    {
        // get install path from file
        string install_path;
        GetInstallPath(install_path);

        // empty means not installed
        if (install_path.length() != 0) {
            std::cout << "tiny-vector-dbms areadly installed in " << install_path << std::endl;
            return false;
        }
        // try install tiny-vector-dbms
        else {
            std::cout << "input the install path (must be a folder): ";
            std::cin >> install_path;
            std::cout << "try install tiny-vector-dbms in " << install_path << std::endl;

            // write install path to storage file
            WriteInstallPath(install_path); 
            // reload install path
            lw->cal_url_util->GetInstallPathFromDisk();

            // install the default db
            CreateSystemDB();

            std::cout << "install successfully in: " << install_path << std::endl;

            return true;
        }
    }

public:

    bool Install()
    {
        return InstallDBMS();
    }

    void CreateSystemDB()
    {   
        std::cout << "----begin Create Base DB----" << std::endl;
        CreateBaseDB();

        std::cout << "----begin init default table----" << std::endl;
        CreateDefaultTableForBaseDb();

    }
    
    // create base db of this system, it will store all db names. so you can search db using it.
    void CreateBaseDB()
    {
        // 1、create db folder
        file_mm->OpenOrMkdir(lw->cal_url_util->GetBaseDbFolder());

        // 2、create db file
        string db_file_uri = lw->cal_url_util->GetDefaultDbFile(DEFAULT_DB_FILE_NAME);
        file_mm->ReadOrCreateFile(db_file_uri).close();

        // 3、write data into db header file
        DB default_db;
        default_db.db_name = DEFAULT_DB_FILE_NAME;
        default_db.db_description = "The base db of this dbms, has a table, which stores all db about this dbms";
        default_db.default_table_header_file_path = lw->cal_url_util->GetTableHeaderFile(DEFAULT_DB_FILE_NAME);
        SerializeDBFile(default_db, db_file_uri);
    }

    /**
     * @brief Create a new database, for sql: CREATE DATABASE db_name;
     * @param db_name The name of the database to be created
     */
    SqlResponse* CreateDB(DB* base_db, string db_name)
    {   
        SqlResponse* sql_response = new SqlResponse();

        if (CheckDBExist(base_db, db_name))
        {
            sql_response->sql_state = FAILURE;
            sql_response->information = "Database " + db_name + " areadly exists";
            return sql_response;
        }

        // write "db_name" to base_db.default_table.db_names
        char* deafult_db_name = new char[db_name.size()];
        memcpy(deafult_db_name, db_name.c_str(), db_name.size());
        Value* insert_value = new Value(deafult_db_name, db_name.size());
        bool insert_result = InsertIntoTable(*base_db, DEFAULT_TABLE_NAME, DEFAULT_TABLE_COLUMN_NAME_ONE, insert_value);
        delete[] deafult_db_name;
        delete insert_value;

        if (!insert_result)
        {
            sql_response->sql_state = FAILURE;
            sql_response->information = "write " + db_name + " to base_db.default_table.db_names failure";
            return sql_response;
        }

        // reload base_db
        OpenDB(base_db, base_db);

        // create db file
        string db_folder_uri = lw->cal_url_util->GetDbFolder(db_name);
        file_mm->OpenOrMkdir(db_folder_uri);
        string db_file_uri = lw->cal_url_util->GetDefaultDbFile(db_name);
        file_mm->ReadOrCreateFile(db_file_uri).close();

        // write data to db file
        DB new_db;
        new_db.db_name = db_name;
        new_db.db_description = "User db, named " + db_name;
        new_db.default_table_header_file_path = lw->cal_url_util->GetTableHeaderFile(db_name);
        SerializeDBFile(new_db, db_file_uri);
        
        // create table header file and default table
        CreateDefaultTable(db_name);

        sql_response->sql_state = SUCCESS;
        return sql_response;
    }

    // TODO
    SqlResponse* DropDB(DB* base_db, string db_name)
    {
        SqlResponse* sql_response = new SqlResponse();
        return sql_response;
    }

    /**
     * Creates a new table in the specified database.
     * @todo: make Update cache -> write to header file and reload db
     * This function performs the following steps:
     * 1. Updates the cache by adding the new table to the database's table list.
     * 2. Creates a new table data file for the table.
     * 3. Creates a data block for each column in the table and writes the address to the table header.
     * 4. Inserts the table into the table header file.
     * 
     * @param db The database in which to create the table.
     * @param table The table to create.
     */
    void CreateTable(DB& db, ColumnTable table)
    {
        // Check if the database is the base database, which cannot be modified
        if (db.db_name == DEFAULT_DB_FILE_NAME)
            throw std::runtime_error("Can not create table in base_db!");

        // 1. Update cache
        db.tables.push_back(table);

        // 2. Create table data file
        file_mm->ReadOrCreateFile(lw->cal_url_util->GetTableDataFile(db.db_name, table.table_name)).close();   // /install/db_name/tables/data/default_table.data
        
        // 3. Create data block for each column and write address to table header
        for (default_amount_type i = 0; i < table.column_size; i++)
        {
            DataBlock data_block;

            // Create a new data block for the column
            default_address_type new_data_block_address = lw->CreateNewBlock(db.db_name, table.columns.column_name_array[i], data_block);
            
            // Initialize the data block
            data_block.field_length = table.columns.column_length_array[i];
            data_block.field_data_nums = 0;
            data_block.next_block_pointer = 0;

            // Write the data block to disk
            lw->ReleaseWritingBlock(db.db_name, table.columns.column_name_array[i], new_data_block_address, data_block);

            // Update the table header with the data block address
            table.columns.column_storage_address_array[i] = new_data_block_address;
        }

        // 4. Insert the table into the table header file
        TableBlock* block = new TableBlock();
        default_address_type block_offset = 0;
        lw->LoadBlockForWrite(db.db_name, DEFAULT_TABLE_NAME, block_offset, *block);
        while (!block->InsertTable(&table))
        {   
            // If the block is full, create a new block and update the previous block's next pointer
            default_address_type cache_next_block_pointer = block->next_block_pointer;
            if (cache_next_block_pointer == 0)
            {
                TableBlock* new_block = new TableBlock();
                default_address_type new_block_offset = lw->CreateNewBlock(db.db_name, DEFAULT_TABLE_NAME, *new_block);

                // Update pre block information
                block->next_block_pointer = new_block_offset;
                lw->ReleaseWritingBlock(db.db_name, DEFAULT_TABLE_NAME, block_offset, *block);
                delete block;

                block = new_block;
                block_offset = new_block_offset;

                block->InsertTable(&table);
                break;
            }

            // Release the current block and load the next block
            lw->ReleaseWritingBlock(db.db_name, DEFAULT_TABLE_NAME, block_offset, *block);
            lw->LoadBlockForWrite(db.db_name, DEFAULT_TABLE_NAME, cache_next_block_pointer, *block);
        }

        // Release the final block
        lw->ReleaseWritingBlock(db.db_name, DEFAULT_TABLE_NAME, block_offset, *block);
        delete block;
    }

    /**
     * Creates the default table for the base database.
     * 
     * This function performs the following steps:
     * 1. Creates the default table header file and data file.
     * 2. Writes data into the table header file, including the data address (which is 0, indicating the data is in the first block of the table data file).
     * 3. Inserts a row into the default table with the base database name.
     */
    void CreateDefaultTableForBaseDb()
    {
        // 1. Create default table header file and data file
        file_mm->OpenOrMkdir(lw->cal_url_util->GetDefaultTablePath(DEFAULT_DB_FILE_NAME));  // /install/base_db/tables
        file_mm->ReadOrCreateFile(lw->cal_url_util->GetTableHeaderFile(DEFAULT_DB_FILE_NAME)).close();   // /install/db_name/tables/default_table.tvdbb
        file_mm->OpenOrMkdir(lw->cal_url_util->GetTableDataFolder(DEFAULT_DB_FILE_NAME));    // /install/db_name/tables/data
        file_mm->ReadOrCreateFile(lw->cal_url_util->GetTableDataFile(DEFAULT_DB_FILE_NAME, DEFAULT_TABLE_DATA_FILE_NAME)).close();   // /install/db_name/tables/data/default_table.data

        // 2. Write data into table header file (data address is 0, because the data of default table must in the first block of table data file)
        // Construct a block to read from or write to disk, construct a column_table (only has one column), and insert to block
        TableBlock block;
        default_address_type block_offset = lw->CreateNewBlock(DEFAULT_DB_FILE_NAME, DEFAULT_TABLE_NAME, block);

        ColumnTable ct;
        ct.table_name = DEFAULT_TABLE_NAME;
        ct.table_type = COMMON;

        DataBlock data_block;
        default_address_type data_block_offset = lw->CreateNewBlock(DEFAULT_DB_FILE_NAME, DEFAULT_TABLE_NAME, data_block);
        ct.InsertColumn(DEFAULT_TABLE_COLUMN_NAME_ONE, VCHAR_T, GetValueTypeLength(ValueType::VCHAR_T), NONE, data_block_offset);
        data_block.field_length = GetValueTypeLength(ValueType::VCHAR_T);
        data_block.field_data_nums = 0;

        block.InsertTable(&ct);
        lw->ReleaseWritingBlock(DEFAULT_DB_FILE_NAME, DEFAULT_TABLE_NAME, data_block_offset, data_block);
        
        lw->ReleaseWritingBlock(DEFAULT_DB_FILE_NAME, DEFAULT_TABLE_NAME, block_offset, block);

        // 3. Reload base db
        DB base_db; 
        base_db.db_name = DEFAULT_DB_FILE_NAME;
        OpenDB(&base_db, &base_db);
        
        // 3. USE base_db INSERT INTO default_table(db_names) VALUES ("base_db")
        string db_name = DEFAULT_DB_FOLDER_NAME;
        char* deafult_db_name = new char[db_name.size()];
        memcpy(deafult_db_name, db_name.c_str(), db_name.size());
        Value* insert_value = new Value(deafult_db_name, db_name.size());
        InsertIntoTable(base_db, DEFAULT_TABLE_NAME, DEFAULT_TABLE_COLUMN_NAME_ONE, insert_value);
        delete[] deafult_db_name;
        delete insert_value;
    }
    
    /**
     * Creates the default table in the specified database.
     * 
     * This function performs the following steps:
     * 1. Creates the default table header file.
     * 2. Writes data into the table header file, including the data address (which is 0, indicating the data is in the first block of the table data file).
     * 3. Creates the table data file and writes the first block of each column.
     * 
     * @param db_name The name of the database in which to create the default table.
     */
    void CreateDefaultTable(string db_name)
    {
        // 1、create default table header file
        file_mm->ReadOrCreateFile(lw->cal_url_util->GetTableHeaderFile(db_name)).close();   // /install/db_name/tables/default_table.tvdbb

        // 2、write data into table header file (data address is 0, because the data of default table must in the first block of table data file)
        // construct a block to read from or write to disk, construct a column_table(only has one column), and insert to block
        TableBlock block;
        default_address_type block_offset = lw->CreateNewBlock(db_name, DEFAULT_TABLE_NAME, block);
        ColumnTable ct;
        ct.table_name = DEFAULT_TABLE_NAME;
        ct.table_type = COMMON;
        
        // 3、create data file and write the first block of each column
        file_mm->OpenOrMkdir(lw->cal_url_util->GetTableDataFolder(db_name));    // /install/db_name/tables/data
        file_mm->ReadOrCreateFile(lw->cal_url_util->GetTableDataFile(db_name, DEFAULT_TABLE_NAME)).close();   // /install/db_name/tables/data/default_table.data
        
        DataBlock data_block;
        default_address_type data_block_offset = lw->CreateNewBlock(db_name, DEFAULT_TABLE_NAME, data_block);
        data_block.field_data_nums = 0;
        data_block.field_length = GetValueTypeLength(ValueType::VCHAR_T);
        lw->ReleaseWritingBlock(db_name, DEFAULT_TABLE_NAME, data_block_offset, data_block);

        // write back table block
        ct.InsertColumn(DEFAULT_TABLE_COLUMN_NAME_TWO, VCHAR_T, GetValueTypeLength(ValueType::VCHAR_T), NONE, data_block_offset);
        block.InsertTable(&ct);
        lw->ReleaseWritingBlock(db_name, DEFAULT_TABLE_NAME, block_offset, block);
    }

    /**
     * Opens a database, either the default database or a user-defined database.
     *
     * @param base_db The cached base database object.
     * @param try_open_db The database to be opened.
     * @return True if the database is opened successfully, false otherwise.
     */
    bool OpenDB(DB* base_db, DB* try_open_db)
    {
        // open base_db
        if (try_open_db->db_name == DEFAULT_DB_FILE_NAME)
        {
            // update base_db
            LoadTables(base_db);
            return true;
        }

        // open user db
        if (!CheckDBExist(base_db, try_open_db->db_name))
        {
            return false;
        }
        LoadTables(try_open_db);
        return true;
    }

    /**
     * Checks if a database with the given name exists in the base database.
     *
     * @param base_db The cached base database object.
     * @param db_name The name of the database to check for existence.
     * @return True if the database exists, false otherwise.
     */
    bool CheckDBExist(DB* base_db, string db_name)
    {
        // Convert the database name to a C-style string
        char* db_name_c = new char[db_name.length()];
        memcpy(db_name_c, db_name.c_str(), db_name.length());
        delete[] db_name_c; // Note: this line is unnecessary, as the memory is immediately freed

        // Create a Value object to represent the database name
        Value* eq_value = new Value(db_name_c, db_name.length());

        // Create a vector to store the result values
        vector<value_tag> result_values;

        // Filter the base database to find values equal to the database name
        FilterEqual(base_db, DEFAULT_TABLE_NAME, DEFAULT_TABLE_COLUMN_NAME_ONE, eq_value, result_values);

        // Delete the Value object
        delete eq_value;

        // Check if any results were found
        if (result_values.size() > 0)
        {
            // If results were found, the database exists
            return true;
        }
        // If no results were found, the database does not exist
        return false;
    }
    
    /**
     * Filters the data in a column of a table to find values that are equal to a given value.
     * @todo now have not use the log!
     * @param db The database to search in.
     * @param table_name The name of the table to search in.
     * @param col_name The name of the column to search in.
     * @param eq_value The value to compare with.
     * @param result_values The vector to store the result values.
     */
    void FilterEqual(DB* db, string table_name, string col_name, Value* eq_value, vector<value_tag>& result_values)
    {
        // Initialize the tag offset to 0
        default_long_int tag_offset = 0;

        // Clear the result values vector
        result_values.clear();

        // Declare variables to store the column table and column data block offset
        ColumnTable* table;
        default_address_type column_data_block_offset;

        // Declare a DataBlock object to store the data block
        DataBlock block;

        // Load the first data block of the column
        bool has_next = LoadFirstDataBlock(*db, table_name, col_name, column_data_block_offset, block);

        // Store the offset of the next data block
        default_address_type cache_next_block_offset = block.next_block_pointer;

        // Filter the data block to find values equal to the given value
        FilterEqualOp(&block, eq_value, result_values, tag_offset);

        // Release the reading block
        lw->ReleaseReadingBlock(db->db_name, table_name, column_data_block_offset, block);

        // Loop until there are no more data blocks
        while(has_next)
        {
            // Load the next data block
            has_next = LoadDataBlocks(*db, table_name, cache_next_block_offset, block);

            // Store the offset of the next data block
            cache_next_block_offset = block.next_block_pointer;

            // Filter the data block to find values equal to the given value
            FilterEqualOp(&block, eq_value, result_values, tag_offset);

            // Release the reading block
            lw->ReleaseReadingBlock(db->db_name, table_name, column_data_block_offset, block);
        }
    }

    /**
     * Filters the data block to find values that are equal to the given value.
     *
     * @param block The data block to filter.
     * @param equal_val The value to compare with.
     * @param result_values The vector to store the result values.
     * @param tag_offset The offset of the tag in the result values.
     */
    void FilterEqualOp(DataBlock* block, Value* equal_val, vector<value_tag>& result_values, default_long_int& tag_offset)
    {
        // Get the number of values in the data block
        default_length_size value_nums = block->field_data_nums;

        // Get the length of the value type
        default_length_size value_length = GetValueTypeLength(equal_val->value_type);

        // Calculate the offset of the value in the data block
        default_address_type value_offset = BLOCK_SIZE - value_length;

        // Loop through each value in the data block
        while (value_nums > 0)
        {
            // Deserialize the value from the data block
            Value* new_val = SerializeValueFromBuffer(equal_val->value_type, block->data, value_offset);

            // Compare the deserialized value with the given value
            if (Compare(equal_val, new_val) == 0)
            {
                // If they are equal, create a new value tag pair and add it to the result values
                value_tag new_val_tag_pair(tag_offset, *new_val);
                result_values.push_back(new_val_tag_pair);
            }
            else
            {
                // If no use, free memory
                delete new_val;
            }

            // Decrement the value count and increment the tag offset
            value_nums--;
            tag_offset++;
        }
    }

    // read table headers stored in default_table.tvdbb
    void LoadTables(DB* db)
    {
        // open db file, like "db.tvbb", and deserialize to get default table name
        string db_file_name = lw->cal_url_util->GetDefaultDbFile(db->db_name);
        DeserializeDBFile(*db, db_file_name);

        // get one slot on memory and load data from disk
        TableBlock block;
        lw->LoadBlockForRead(db->db_name, DEFAULT_TABLE_NAME, 0, block);

        // deserialize the tables in tables block and cache
        for (default_amount_type i = 0; i < block.table_amount; i++)
        {
            ColumnTable table;
            table.Deserialize(block.data, block.tables_begin_address[i]);
            db->tables.push_back(table);
        }

        lw->ReleaseReadingBlock(db->db_name, DEFAULT_TABLE_NAME, 0, block);

        // serialize next block, if exists
        while (block.next_block_pointer != 0x0)
        {
            // read next block
            lw->LoadBlockForRead(db->db_name, DEFAULT_TABLE_NAME, block.next_block_pointer, block);
            // deserialize the tables in tables block and cache
            for (default_amount_type i = 0; i < block.table_amount; i++)
            {
                ColumnTable* table = new ColumnTable();
                table->Deserialize(block.data, block.tables_begin_address[i]);
                db->tables.push_back(*table);
            }
            lw->ReleaseReadingBlock(db->db_name, DEFAULT_TABLE_NAME, block.next_block_pointer, block);
        }
    }
    
    // return: have next block
    bool LoadFirstDataBlock(DB& db, string table_name, string col_name, default_length_size& first_block_offset, DataBlock& block)
    {
        ColumnTable* table = nullptr;
        default_address_type column_offset;
        if (!GetColumn(db, table_name, col_name, table, column_offset))
        {
            throw std::runtime_error("DB has no table named " + table_name + " or col named " + col_name);
        }

        // read the first data block of the column
        first_block_offset = table->columns.column_storage_address_array[column_offset];
        lw->LoadBlockForRead(db.db_name, table_name, first_block_offset, block);
        
        // TODO: close block!

        // return has next block
        return block.next_block_pointer == 0x0;
    }

    // return: have next block
    bool LoadDataBlocks(DB db, string table_name, default_length_size block_address, DataBlock& block)
    {
        if (block_address == 0x0)
        {
            throw std::runtime_error("Can not load next block, because its null");
        }

        // read the next block
        lw->LoadBlockForRead(db.db_name, table_name, block_address, block);

        // TODO: close block!

        // return has next block
        return block.next_block_pointer == 0x0;
    }

    bool InsertIntoTable(DB& db, string table_name, string column_name, Value* insert_value)
    {   
        // check table and column exists.
        ColumnTable* table = nullptr;
        default_address_type column_offset;

        if (!GetColumn(db, table_name, column_name, table, column_offset))
        {
            return false;
            // throw std::runtime_error("DB has no table named " + table_name + " or col named " + column_name);
        }

        // check value type
        if (insert_value->value_type != table->columns.column_type_array[column_offset])
        {
            return false;
        }
        default_length_size data_size = insert_value->GetValueLength();
        char* value_c = new char[data_size];
        insert_value->Serialize(value_c, 0);

        // open a data block
        DataBlock* data_block = new DataBlock();
        default_address_type read_offset = table->columns.column_storage_address_array[column_offset];
        lw->LoadBlockForWrite(db.db_name, table->table_name, read_offset, *data_block);

        // get the last block
        while (data_block->next_block_pointer != 0x0)
        {   
            // cache the next block offset
            default_address_type cached_next_block_offset = data_block->next_block_pointer;

            // release the using block
            lw->ReleaseWritingBlock(db.db_name, table->table_name, read_offset, *data_block);

            // open the next block
            read_offset = cached_next_block_offset;
            lw->LoadBlockForWrite(db.db_name, table->table_name, read_offset, *data_block);
        }
        // check space
        if (!data_block->HaveSpace(data_size))
        {
            // create one new block
            DataBlock* new_data_block = new DataBlock();
            default_address_type new_block_offset = lw->CreateNewBlock(db.db_name, column_name, *new_data_block);

            // update the last block
            data_block->next_block_pointer = new_block_offset;
            lw->ReleaseWritingBlock(db.db_name, table->table_name, read_offset, *data_block);
            delete data_block;
            data_block = new_data_block;
            read_offset = new_block_offset;
        }

        // insert data
        data_block->InsertData(value_c, data_size);

        // write back
        lw->ReleaseWritingBlock(db.db_name, table->table_name, read_offset, *data_block);

        return true;
    }

    /**
     * Retrieves a column from a table in the database.
     *
     * @param db The database to search in.
     * @param table_name The name of the table to search in.
     * @param column_name The name of the column to retrieve.
     * @param table A pointer to the ColumnTable object that will be populated with the retrieved column.
     * @param column_offset The offset of the column in the table.
     *
     * @return True if the column is found, false otherwise.
     */
    bool GetColumn(DB& db, string table_name, string column_name, ColumnTable*& table, default_address_type& column_offset)
    {
        // Check if the table exists in the database
        table = nullptr;
        for (default_length_size i = 0; i < db.tables.size(); i++)
        {
            // Compare the table name with the given table name
            if (db.tables[i].table_name == table_name)
            {
                // If a match is found, set the table pointer and break the loop
                table = &db.tables[i];
                break;
            }
        }
        // If the table is not found, return false
        if (table == nullptr)
        {
            return false;
        }

        // Initialize the column offset to 0
        column_offset = 0;
        // Iterate through the columns in the table
        for(default_amount_type i = 0; i < table->column_size; i++)
        {
            // Compare the column name with the given column name
            if (table->columns.column_name_array[i] == column_name)
            {
                // If a match is found, set the column offset and break the loop
                column_offset = i;
                break;
            }
        }
        // If the column is not found, return false
        if (column_offset >= table->column_size)
        {
            return false;
        }

        // If the column is found, return true
        return true;
    }

    void DeleteRecord()
    {

    }

    void UpdateRecord()
    {
        
    }

    // for sql: SELECT column_name FROM table_name
    // void SelectFromTable(string db_name, string table_name, string column_name, DataBlock& block)
    // {
    //     // check table and column exists.
    //     ColumnTable table;
    //     if (!CheckTableExist(db_name, table_name, table))
    //     {
    //         throw std::runtime_error("table: " + table_name + " not exist in db: " + db_name);
    //     }

    //     Columns column;
    //     if (!CheckColumnExist(table, column_name, column)) 
    //     {
    //         throw std::runtime_error("column: " + column_name + "not exists in table: " + table_name);
    //     }

    //     // string data_file_uri = GetAndCreateDataFile(GetAndCreateDataFolderPath(GetAndCreateDbFolder(db_name)), table_name);
    //     // default_address_type read_offset = column.column_storage_address_array[0];
    //     // ReadOneDataBlock(data_file_uri, read_offset, block);
    // }

    /**
     * @brief Checks if a database exists
     * @param db_name The name of the database to check
     * @return True if the database exists, false otherwise
    */
    // bool CheckDbExist(string db_name)
    // {
    //     // get default table of base db
    //     ColumnTable default_table;
    //     GetDefaultTable(DEFAULT_DB_FOLDER_NAME, default_table);

    //     // check whether input db_name in it
    //     for (default_amount_type i = 0; i < default_table.column_size; i++)
    //     {
    //         if (default_table.columns.column_name_array[i] == db_name)
    //         {
    //             return true;
    //         }
    //     }
    //     return false;
    // }

    /**
     * @brief Checks if a table exists in a database.
     *
     * @param db_name The name of the database.
     * @param table_name The name of the table to check.
     * @param table A reference to a ColumnTable object that will be populated with the table's metadata if it exists.
     *
     * @return True if the table exists, false otherwise.
     *
     * @example
     * ColumnTable table;
     * if (CheckTableExist("my_database", "my_table", table)) {
     *     std::cout << "Table exists!" << std::endl;
     * } else {
     *     std::cout << "Table does not exist." << std::endl;
     * }
    */
    // bool CheckTableExist(string db_name, string table_name, ColumnTable& table)
    // {
    //     // open db file, like "db.tvbb", and deserialize to get default table name
    //     string db_file_name = GetAndCreateDbFolder(db_name) + "/"+ db_name + DB_FILE_SUFFIX;

    //     DB db_file;
    //     DeserializeDBFile(db_file, db_file_name);

    //     // open default table file, like "default_table.tvdbb"
    //     TableBlock block;

    //     // read db headers file to block, offset is 0, because the first block must at 0 offset
    //     ReadOneTableBlock(db_file.default_table_header_file_path, 0, block);
    //     block.DeserializeFromBuffer(block.data);
    //     // deserialize the tables in tables block and check
    //     for (default_amount_type i = 0; i < block.table_amount; i++)
    //     {
    //         table.Deserialize(block.data, block.tables_begin_address[i]);
    //         std::cout << "found table in .tvdbb: " << table.table_name << std::endl;
    //         if (table.table_name == table_name)
    //         {
    //             return true;
    //         }
    //     }

    //     // if not found, find in next block.
    //     while (block.next_block_pointer != 0)
    //     {
    //         // read next block
    //         ReadOneTableBlock(db_file.default_table_header_file_path, block.next_block_pointer, block);   
    //         block.DeserializeFromBuffer(block.data);
    //         // deserialize the tables in tables block and check
    //         for (default_amount_type i = 0; i < block.table_amount; i++)
    //         {
    //             table.Deserialize(block.data, block.tables_begin_address[i]);
    //             if (table.table_name == table_name)
    //             {
    //                 return true;
    //             }
    //         }
    //     }
        
    //     // if still not found, return false
    //     return false;
    // }

    /**
     * @brief Checks if a column exists in a table.
     *
     * @param ct The ColumnTable object that contains the table's metadata.
     * @param column_name The name of the column to check.
     * @param column A reference to a Columns object that will be populated with the column's metadata if it exists.
     *
     * @return True if the column exists, false otherwise.
     *
     * @example
     * ColumnTable table;
     * Columns column;
     * if (CheckColumnExist(table, "my_column", column)) {
     *     std::cout << "Column exists!" << std::endl;
     *     std::cout << "Column name: " << column.column_name_array[0] << std::endl;
     *     std::cout << "Column type: " << column.column_type_array[0] << std::endl;
     * } else {
     *     std::cout << "Column does not exist." << std::endl;
     * }
    */
    bool CheckColumnExist(ColumnTable ct, string column_name, Columns& column)
    {
        for (default_amount_type i = 0; i < ct.column_size; i++)
        {   
            if (ct.columns.column_name_array[i] == column_name)
            {   
                column.column_name_array = new string[0];
                column.column_name_array[0] = ct.columns.column_name_array[i];
                column.column_type_array = new default_enum_type[0];
                column.column_type_array[0] = ct.columns.column_type_array[i];
                column.column_length_array = new default_length_size[0];
                column.column_length_array[0] = ct.columns.column_length_array[i];
                column.column_index_type_array = new default_enum_type[0];
                column.column_index_type_array[0] = ct.columns.column_index_type_array[i];
                column.column_storage_address_array = new default_address_type[0];
                column.column_storage_address_array[0] = ct.columns.column_storage_address_array[i];
                return true;
            }
            
        }

        return false;
    }

public:
    Operator()
    {
        file_mm = new FileManagement();
        bfmm = new BlockFileManagement();
        lw = new LockWatcher(SLOT_AMOUNT);
    }

    /**
     * @brief Cal and return the default table data path base on the base db path.
     * @return default table data path of the input db path.
     */
    // string GetDefaultTableDataFolderOfBaseDbPath()
    // {   
    //     string db_folder = GetBaseDbFolder();
    //     string table_path = GetDefaultTablePath(db_folder);

    //     // create table data folder
    //     string default_table_data_folder = table_path + "/" + DEFAULT_TABLE_DATA_FOLDER;
    //     file_mm->OpencvDirAndMkdir(default_table_data_folder);
    //     return default_table_data_folder;
    // }
    
    /**
     * @brief Cal and return the default table data path base on the input db path.
     * @param table_path db folder path
     * @return default table data folder path of the input db path.
     */
    // string GetAndCreateDataFolderPath(string db_path)
    // {   
    //     string table_path = GetDefaultTablePath(db_path);
    //     string default_table_data_folder = table_path + "/" + DEFAULT_TABLE_DATA_FOLDER;
    //     file_mm->OpencvDirAndMkdir(default_table_data_folder);
    //     return default_table_data_folder;
    // }

    /**
     * @brief Get the data file path of input table_name. if not exist, then create it.
     * @param data_folder_path data file store folder
     * @param table_name table name
     * @return data file path of input table_name
     */
    // string GetAndCreateDataFile(string data_folder_path, string table_name)
    // {
    //     string table_data_file = data_folder_path;
    //     table_data_file += "/";
    //     table_data_file += table_name;
    //     table_data_file += TABLE_DATA_FILE_SUFFIX;
    //     file_mm->ReadOrCreateFile(data_folder_path +);
    //     lw.
    //     return table_data_file;
    // }

    // store a db object to file
    void SerializeDBFile(DB& db, string file_path)
    {   
        char endl = '\n';
        file_mm->WriteFile(file_path, db.db_name.c_str(), db.db_name.length());
        file_mm->WriteFileAppend(file_path, &endl, 1);
        file_mm->WriteFileAppend(file_path, db.db_description.c_str(), db.db_description.length());
        file_mm->WriteFileAppend(file_path, &endl, 1);
        file_mm->WriteFileAppend(file_path, db.default_table_header_file_path.c_str(), db.default_table_header_file_path.length());
        file_mm->WriteFileAppend(file_path, &endl, 1);
    }
    
    // load a db object from filef
    void DeserializeDBFile(DB& db, string file_path)  
    {  
        std::ifstream file_read = file_mm->ReadOrCreateFile(file_path);
        getline(file_read, db.db_name);
        getline(file_read, db.db_description);
        getline(file_read, db.default_table_header_file_path);
        file_read.close();
    } 

};

}

#endif // VDBMS_SQL_EXECUTER_OPERATOR_H_