// Copyright (c) 2024 by dingning
//
// file  : operator.h
// since : 2024-07-19
// desc  : this file contains all operator to execute one sql.


#ifndef VDBMS_SQL_EXECUTER_OPERATOR_H_
#define VDBMS_SQL_EXECUTER_OPERATOR_H_

#include <iostream>

#include "../../config.h"
#include "../../meta/db/db.h"
#include "../../storage/file_management.h"
#include "../../memory/memory_management.h"
// memory buffer poll
#include "../../storage/memory/lock_watcher.h"
// table header & table data
#include "../../meta/table/column_table.h"
#include "../../meta/block/table_block.h"
#include "../../meta/block/data_block.h"
#include "../../storage/block_file_management.h"
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
    // string GetInstallPath() 
    // {
    //     // string install_path;
        
    //     // std::ifstream file_read = file_mm->ReadOrCreateFile(INSTALL_PATH_CACHE_FILE);
    //     // getline(file_read, install_path);
    //     // file_read.close();

    //     return lw->cal_url_util->GetInstallPath();
    // }

    // /**
    //  * @brief Get the folder where installed base db,
    //  * @return The folder where installed base db.
    //  */ 
    // string GetBaseDbFolder()
    // {
    //     return GetAndCreateDbFolder(DEFAULT_DB_FOLDER_NAME);
    // }

    // /**
    //  * @brief Get the folder where installed input db. Read base install path from file
    //  * @param db_name db name
    //  * @return  The folder where installed input db.
    //  */
    // string GetAndCreateDbFolder(string db_name)
    // {
    //     if (GetInstallPath() == "")
    //     {
    //         throw std::runtime_error("dbms has not been installed!");
    //     }
    //     string db_folder = GetInstallPath() + "/" + db_name;
    //     file_mm->OpencvDirAndMkdir(db_folder);
    //     return db_folder;
    // }

    // /**
    //  * @brief Get the db file of input db.
    //  * @param db_name db name
    //  * @return  The path where is he db file of input db.
    //  */
    // string GetAndCreateDefaultDbFile(string db_name)
    // {   
    //     // create db file, db file has the same name as db folder
    //     string db_file_name = GetAndCreateDbFolder(db_name) + "/" + db_name + DB_FILE_SUFFIX;
    //     file_mm->ReadOrCreateFile(db_file_name).close();
    //     return db_file_name;
    // }

    // /**
    //  * @brief Cal and return the default table path base on the input db path.
    //  * @param db_path db folder path
    //  * @return default table path of the input db path.
    //  */
    // string GetDefaultTablePath(string db_path)
    // {   
    //     // create table folder
    //     string default_table_folder = db_path + "/" + DEFAULT_TABLE_FOLDER;
    //     file_mm->OpencvDirAndMkdir(default_table_folder);
    //     return default_table_folder;
    // }

    // /**
    //  * @brief Cal and return the default table header file path base on the input tables path.
    //  * @param tables_path tables folder path, like: install_path/base_db/tables
    //  * @return default table path of the input db path.
    //  */
    // string GetAndCreateDefaultTableFile(string tables_path)
    // {
    //     string default_table_header_uri = tables_path + "/" + DEFAULT_TABLE_NAME + TABLE_FILE_SUFFIX;
    //     file_mm->ReadOrCreateFile(default_table_header_uri).close();
    //     return default_table_header_uri;
    // }

    // /**
    //  * Reads a table block from a file
    //  * @param table_file_uri The URI of the table file
    //  * @param offset The offset of the block in the file
    //  * @param new_block The TableBlock object to store the read data
    // */
    // void ReadOneTableBlock(string table_file_uri, default_address_type offset, TableBlock& new_block)
    // {   
    //     // read from disk
    //     fstream file_stream;
    //     bfmm->OpenTableFile(table_file_uri, file_stream);    // open table header file, like "test.tvdbb"
    //     bfmm->ReadFromFile(file_stream, offset, new_block.data);
    //     new_block.DeserializeFromBuffer(new_block.data);
    //     file_stream.close();
    // }

    // /**
    //  * Reads a data block from a file
    //  * @param data_file_uri The URI of the data file
    //  * @param offset The offset of the block in the file
    //  * @param new_block The DataBlock object to store the read data
    // */
    // void ReadOneDataBlock(string data_file_uri, default_address_type offset, DataBlock& new_block)
    // {
    //     fstream file_stream;
    //     bfmm->OpenDataFile(data_file_uri, file_stream);    // open table header file, like "test.data"
    //     bfmm->ReadFromFile(file_stream, offset, new_block.data);
    //     new_block.DeserializeFromBuffer(new_block.data);
    //     file_stream.close();
    // }

    // void WriteBackTableBlock(string table_file_uri, default_address_type offset, TableBlock& block)
    // {
    //     fstream file_stream;
    //     bfmm->OpenTableFile(table_file_uri, file_stream);   // open table header file, like "test.tvdbb"   
    //     bfmm->WriteBackBlock(file_stream, offset, block.data);
    //     file_stream.close();
    // }

    // void WriteBackDataBlock(string data_file_uri, default_address_type offset, DataBlock& block)
    // {
    //     block.Serialize();
    //     fstream file_stream;
    //     bfmm->OpenDataFile(data_file_uri, file_stream);    // open data file, like "test.data"   
    //     bfmm->WriteBackBlock(file_stream, offset, block.data);
    //     file_stream.close();
    // }
    
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

    /*
        CREATE DATABASE base_db;
        USE base_db
        CREATE TABLE default_table (
            db_names VCHAR(20)
        );
    */
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
        string db_path_uri = lw->cal_url_util->GetBaseDbFolder();
        file_mm->OpenOrMkdir(db_path_uri);

        // 2、create db header file
        string db_file_uri = lw->cal_url_util->GetDefaultDbFile(DEFAULT_DB_FILE_NAME);
        file_mm->ReadOrCreateFile(db_file_uri).close();

        // 3、write data into db header file
        DB default_db;
        default_db.db_name = DEFAULT_DB_FILE_NAME;
        default_db.db_description = "The base db of this dbms, has a table, which stores all db about this dbms";
        default_db.default_table_header_file_path = db_file_uri;
        SerializeDBFile(default_db, db_file_uri);
    }

    /**
     * @todo: 未完成，需要使用缓存的base_db信息
     * @brief Create a new database, for sql: CREATE DATABASE db_name;
     * @param db_name The name of the database to be created
     */
    void CreateDB(string db_name)
    {
        // 1、get or create db folder and file
        // string db_path_uri = GetAndCreateDbFolder(db_name);
        // string db_file_uri = GetAndCreateDefaultDbFile(db_name);

        // DB default_db;
        // default_db.db_name = db_name;
        // default_db.default_table_header_file_path = GetAndCreateDefaultTableFile(GetDefaultTablePath(db_path_uri));
        // SerializeDBFile(default_db, db_file_uri);

        // // 2、insert one row in base_db.default_table, means add one db
        // char* insert_db_name = new char[VCHAR_LENGTH];
        // memcpy(insert_db_name, &db_name, VCHAR_LENGTH);
        // // TODO: cache base_db
        // // InsertIntoTable(DEFAULT_DB_FOLDER_NAME, DEFAULT_TABLE_NAME, DEFAULT_TABLE_COLUMN_NAME_ONE, insert_db_name);

        // // 3、Create default table
        // CreateDefaultTable(db_name);
    }

    // for sql : USE db_name CREATE TABLE table_name(column1, column2, ...)
    void CreateTable(DB& db, ColumnTable table)
    {
        // insert one record in default table
        char* insert_db_name = new char[VCHAR_LENGTH];
        memcpy(insert_db_name, &table.table_name, VCHAR_LENGTH);
        InsertIntoTable(db, DEFAULT_TABLE_NAME, DEFAULT_TABLE_COLUMN_NAME_TWO, insert_db_name);

        // update cache
        db.tables.push_back(table);

        // create table data file
        // string data_file = GetAndCreateDataFile(GetAndCreateDbFolder(db.db_name), table.table_name);

        // create data block for each column, and write address to table header
        for (default_amount_type i = 0; i < table.column_size; i++)
        {
            DataBlock data_block;

            default_address_type new_data_block_address = lw->CreateNewBlock(db.db_name, table.columns.column_name_array[i], data_block);
            
            // ReadOneDataBlock(data_file, new_data_block_address, data_block);
            data_block.field_length = table.columns.column_length_array[i];
            data_block.field_data_nums = 0;
            data_block.next_block_pointer = 0;

            lw->ReleaseWritingBlock(db.db_name, table.columns.column_name_array[i], new_data_block_address, data_block);

            // WriteBackDataBlock(table_header_file, block_address, data_block);
            table.columns.column_storage_address_array[i] = new_data_block_address;
        }

        // insert into the table header file
        TableBlock* block = new TableBlock();
        default_address_type block_offset = 0;
        lw->LoadBlockForWrite(db.db_name, DEFAULT_TABLE_NAME, block_offset, *block);
        while (!block->InsertTable(&table))
        {   
            default_address_type cache_next_block_pointer = block->next_block_pointer;
            if (cache_next_block_pointer == 0)
            {
                TableBlock* new_block = new TableBlock();
                default_address_type new_block_offset = lw->CreateNewBlock(db.db_name, DEFAULT_TABLE_NAME, *new_block);

                // update pre block information
                block->next_block_pointer = new_block_offset;
                lw->ReleaseWritingBlock(db.db_name, DEFAULT_TABLE_NAME, block_offset, *block);
                delete block;

                block = new_block;
                block_offset = new_block_offset;

                block->InsertTable(&table);
                break;
            }

            lw->ReleaseWritingBlock(db.db_name, DEFAULT_TABLE_NAME, block_offset, *block);
            lw->LoadBlockForWrite(db.db_name, DEFAULT_TABLE_NAME, cache_next_block_pointer, *block);
        }

        lw->ReleaseWritingBlock(db.db_name, DEFAULT_TABLE_NAME, block_offset, *block);
        delete block;
    }
    
    // create a special default table for base db, this table will store all db's name.
    void CreateDefaultTableForBaseDb()
    {
        // 1、create default table header file
        file_mm->OpenOrMkdir(lw->cal_url_util->GetDefaultTablePath(DEFAULT_DB_FILE_NAME)); // /install/base_db/tables
        file_mm->ReadOrCreateFile(lw->cal_url_util->GetTableHeaderFile(DEFAULT_DB_FILE_NAME)).close(); // /install/base_db/tables/default_table.tvdbb

        // 2、write data into table header file (data address is 0, because the data of default table must in the first block of table data file)
        // construct a block to read from or write to disk, construct a column_table(only has one column), and insert to block
        TableBlock block;
        default_address_type block_offset = lw->CreateNewBlock(DEFAULT_DB_FILE_NAME, DEFAULT_TABLE_NAME, block);

        ColumnTable ct;
        ct.table_name = DEFAULT_TABLE_NAME;
        ct.table_type = COMMON;
        ct.InsertColumn(DEFAULT_TABLE_COLUMN_NAME_ONE, VCHAR, VCHAR_LENGTH, NONE, 0);    // insert one column header, column name is db_names
        block.InsertTable(&ct);    // insert table header data to block

        lw->ReleaseWritingBlock(DEFAULT_DB_FILE_NAME, DEFAULT_TABLE_NAME, block_offset, block);

        // reload base db
        DB base_db;
        OpenDB(base_db);
        
        // 3、USE base_db INSERT INTO default_table(db_names) VALUES ("base_db")
        string db_name = DEFAULT_DB_FOLDER_NAME;
        char* deafult_db_name = new char[ct.columns.column_length_array[ct.column_size - 1]];
        strcpy(deafult_db_name, db_name.c_str());
        InsertIntoTable(base_db, DEFAULT_TABLE_NAME, DEFAULT_TABLE_COLUMN_NAME_ONE, deafult_db_name);
        delete[] deafult_db_name;

    }

    // this is the function is used when creating user table
    void CreateDefaultTable(string db_name)
    {
        // 1、create default table header file
        // string tables_path = GetDefaultTablePath(GetAndCreateDbFolder(db_name));
        // string default_table_file_uri = GetAndCreateDefaultTableFile(tables_path);

        // 2、write data into table header file (data address is 0, because the data of default table must in the first block of table data file)
        // construct a block to read from or write to disk, construct a column_table(only has one column), and insert to block
        TableBlock block;
        default_address_type block_offset = lw->CreateNewBlock(db_name, DEFAULT_TABLE_NAME, block);
        ColumnTable ct;
        ct.table_name = DEFAULT_TABLE_NAME;
        ct.table_type = COMMON;
        ct.InsertColumn(DEFAULT_TABLE_COLUMN_NAME_TWO, VCHAR, VCHAR_LENGTH, NONE, 0);    // insert one column header, column name is db_names
        block.InsertTable(&ct);    // insert table header data to block
        // write back
        lw->ReleaseWritingBlock(db_name, DEFAULT_TABLE_NAME, block_offset, block);
        
        // 3、store one row, which is "default_table", means the first table is default_table
        string table_name = DEFAULT_TABLE_NAME;
        char* deafult_db_name = new char[ct.columns.column_length_array[ct.column_size - 1]];
        strcpy(deafult_db_name, table_name.c_str());
        
        // 3、INSERT INTO default_table(db_names) VALUES (data) FROM base_db
        // InsertIntoTable(DEFAULT_DB_FOLDER_NAME, DEFAULT_TABLE_NAME, DEFAULT_TABLE_COLUMN_NAME_ONE, deafult_db_name);

        delete[] deafult_db_name;
    }
    
    // // get the default table of input db
    // void GetDefaultTable(string db_name, ColumnTable& default_table)
    // {
    //     if (!CheckTableExist(db_name, DEFAULT_TABLE_NAME, default_table))
    //     {
    //         throw std::runtime_error("db_name 's default_table not exist");
    //     }
    // }

    /**
     * @brief first function, used when create connection between client and server
     */
    bool OpenDB(DB& db)
    {
        if (db.db_name == DEFAULT_DB_FILE_NAME)
        {
            OpenDefaultDB(db);
            return true;
        }

        // if (CheckDbExist(db.db_name))   // load base db's default table's data, scan and check whether the input db exist.
        // {
            OpenUserDB(db);
            return true;
        // }

        return false;
    }

    // default db has only  default tables, the data of it are user db's name of this system, so here have no need to read default table's data
    // to check one db whether exists, can cache (base db)->(default table)->(data)
    void OpenDefaultDB(DB& db)
    {
        LoadTables(db);
    }
    
    // all table's header are stored on default_table.tvdbb file, the data of default table now has no use.
    void OpenUserDB(DB& db)
    {
        LoadTables(db);

        // // open default table, read data, load user tables
        // DataBlock block;
        // string data_file_url;
        // bool has_next_block = LoadFirstDataBlock(db, DEFAULT_TABLE_DATA_FILE_NAME, DEFAULT_TABLE_COLUMN_NAME_TWO, block, data_file_url);

        // // parse the first block
        // default_length_size records_num = block.field_data_nums;
        // char* table_name = new char[block.field_length];
        // while(records_num > 0)
        // {
        //     // get table name
        //     memcpy(table_name, block.data + (BLOCK_SIZE - records_num * block.field_length), block.field_length);
        //     string table_name_str(table_name);

        //     // load table header
        //     ColumnTable new_table;
        //     records_num--;
        // }
        // delete[] table_name;

        // // parse other block, if exists
        // while(has_next_block && LoadDataBlocks(data_file_url, block))
        // {   

        // }
    }

    // read table headers stored in default_table.tvdbb
    void LoadTables(DB& db)
    {
        // open db file, like "db.tvbb", and deserialize to get default table name
        string db_file_name = lw->cal_url_util->GetDefaultDbFile(db.db_name);
        DeserializeDBFile(db, db_file_name);

        // get one slot on memory and load data from disk
        TableBlock block;
        lw->LoadBlockForRead(db.db_name, DEFAULT_TABLE_NAME, 0, block);

        // deserialize the tables in tables block and cache
        for (default_amount_type i = 0; i < block.table_amount; i++)
        {
            ColumnTable table;
            table.Deserialize(block.data, block.tables_begin_address[i]);
            db.tables.push_back(table);
        }

        lw->ReleaseReadingBlock(db.db_name, "", 0, block);

        // serialize next block, if exists
        while (block.next_block_pointer != 0x0)
        {
            // read next block
            lw->LoadBlockForRead(db.db_name, "", block.next_block_pointer, block);
            // deserialize the tables in tables block and cache
            for (default_amount_type i = 0; i < block.table_amount; i++)
            {
                ColumnTable* table = new ColumnTable();
                table->Deserialize(block.data, block.tables_begin_address[i]);
                db.tables.push_back(*table);
            }
            lw->ReleaseReadingBlock(db.db_name, "", block.next_block_pointer, block);
        }
    }
    
    // return: have next block
    bool LoadFirstDataBlock(DB db, string table_name, string col_name, DataBlock& block, string& data_file_url)
    {
        ColumnTable* table = nullptr;

        // check table exist in db
        for (auto& item: db.tables)
        {
            if (item.table_name == table_name)
            {
                table = &item;
            }
        }

        // if not found table
        if (table == nullptr)
            throw std::runtime_error("DB has no table named " + table_name);

        // get column information
        default_length_size column_offset = 0;
        while (column_offset < table->column_size)
        {
            if (table->columns.column_name_array[column_offset] == col_name)
            {
                break;
            }
            column_offset++;
        }

        // if not found column
        if (column_offset >= table->column_size)
        {
            throw std::runtime_error("table has no column " + col_name);
        }

        // read the first data block of the column
        lw->LoadBlockForRead(db.db_name, table_name, table->columns.column_storage_address_array[column_offset], block);
        
        // TODO: close block!

        // return has next block
        return block.next_block_pointer == 0x0;
    }

    // return: have next block
    bool LoadDataBlocks(DB db, string table_name, DataBlock& block)
    {
        // cache next block address
        default_address_type next_block_address = block.next_block_pointer;

        if (next_block_address == 0x0)
        {
            throw std::runtime_error("Can not load next block, because its null");
        }

        // read the next block
        lw->LoadBlockForRead(db.db_name, table_name, next_block_address, block);

        // TODO: close block!

        // return has next block
        return block.next_block_pointer == 0x0;
    }

    // mainly used when insert data to default table of base db, because the default table of base db will store all db names but not 
    void InsertIntoTableWithoutCheck(string db_name, string table_name, string column_name, char* data)
    {
        
    }

    void InsertIntoTable(DB db, string table_name, string column_name, char* data)
    {   
        // check table and column exists.
        ColumnTable* table = nullptr;
        for (auto item: db.tables)
        {
            if (item.table_name == table_name)
            {
                table = &item;
                break;
            }
        }

        if (table == nullptr)
        {
            throw std::runtime_error("Can not find " + table_name + "on table: " + db.db_name);
        }

        default_amount_type column_offset = 0;
        for(default_amount_type i = 0; i < table->column_size; i++)
        {
            if (table->columns.column_name_array[i] == column_name)
            {
                column_offset = i;
                break;
            }
        }

        if (column_offset >= table->column_size)
        {
            throw std::runtime_error("column: " + column_name + "not exists in table: " + table_name);
        }

        // open a data block
        DataBlock data_block;
        default_address_type read_offset = table->columns.column_storage_address_array[column_offset];
        lw->LoadBlockForWrite(db.db_name, table->table_name, read_offset, data_block);

        // get one data block has space
        while (!data_block.HaveSpace())
        {   
            read_offset = data_block.next_block_pointer;

            // if has no next block, then create one new block to store
            if (read_offset == 0x0)
            {
                read_offset = lw->CreateNewBlock(db.db_name, column_name, data_block);
            }
        }

        // insert data
        data_block.field_length = table->columns.column_length_array[column_offset];
        data_block.next_block_pointer = 0x0;    // must insert at last block, so set it's next_block_pointer as 0
        data_block.InsertData(data);

        // write back
        lw->ReleaseWritingBlock(db.db_name, table->table_name, read_offset, data_block);
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