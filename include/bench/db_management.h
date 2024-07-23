// Copyright (c) 2024 by dingning
//
// file  : db_management.h
// since : 2024-07-19
// desc  : 
/*
install folder
--db_name
----






*/

#ifndef VDBMS_BENCH_DB_MANAGEMENT_H_
#define VDBMS_BENCH_DB_MANAGEMENT_H_

#include <iostream>

#include "../config.h"
#include "../meta/db/db.h"
#include "../storage/file_management.h"
#include "../memory/memory_management.h"
#include "../meta/table/column_table.h"
#include "../meta/block/table_block.h"
#include "../storage/table_file_management.h"

namespace tiny_v_dbms {


class DBManagement {
private:
    FileManagement* file_mm;

    // get install path from file
    string GetInstallPath() 
    {
        string install_path;

        std::ifstream file_read = file_mm->ReadOrCreateFile(INSTALL_PATH_CACHE_FILE);
        getline(file_read, install_path);
        file_read.close();

        return install_path;
    }

    /**
     * @brief Get the folder where installed base db,
     * @return The folder where installed base db.
     */ 
    string GetBaseDbFolder()
    {
        return GetOrCreateDbFolder(DEFAULT_DB_FOLDER_NAME);
    }

    /**
     * @brief Get the folder where installed input db. Read base install path from file
     * @param db_path db folder path
     * @return  The folder where installed input db.
     */
    string GetOrCreateDbFolder(string db_name)
    {
        string db_folder = GetInstallPath() + "/" + db_name;
        file_mm->OpencvDirAndMkdir(db_folder);
        return db_folder;
    }

    /**
     * @brief Get the db file of input db.
     * @param db_name db name
     * @return  The path where is he db file of input db.
     */
    string GetOrCreateDefaultDbFile(string db_name)
    {   
        // create db file, db file has the same name as db folder
        string db_file_name = GetOrCreateDbFolder(db_name) + "/" + DEFAULT_DB_FILE_NAME + DB_FILE_SUFFIX;
        file_mm->ReadOrCreateFile(db_file_name).close();
        return db_file_name;
    }

    /**
     * @brief Cal and return the default table path base on the input db path.
     * @param db_path db folder path
     * @return default table path of the input db path.
     */
    string GetDefaultTablePath(string db_path)
    {   
        // create table folder
        string default_table_folder = db_path + "/" + DEFAULT_TABLE_FOLDER;
        file_mm->OpencvDirAndMkdir(default_table_folder);
        return default_table_folder;
    }

    /**
     * @brief Cal and return the default table header file path base on the input tables path.
     * @param tables_path tables folder path, like: install_path/base_db/tables
     * @return default table path of the input db path.
     */
    string GetAndCreateDefaultTableFile(string tables_path)
    {
        string default_table_header_uri = tables_path + "/" + DEFAULT_TABLE_NAME + TABLE_FILE_SUFFIX;
        file_mm->ReadOrCreateFile(default_table_header_uri).close();
        return default_table_header_uri;
    }

public:

    /*
        CREATE DATABASE base_db;
        CREATE TABLE default_table (
            db_names VCHAR(20)
        )
        FROM base_db;
    */
    void CreateSystemDB()
    {
        CreateBaseDB();
        CreateDefaultTableForBaseDb();

        // 6、create table data folder


        // 7、create default table data file

        // 8、write default table data (write a empty block, but with a not empty header)
    }
    
    // for sql: CREATE DATABASE db_name;
    void CreateDB(string db_name)
    {
        // create db folder

    }

    // create base db of this system, it will store all db names. so you can search db using it.
    void CreateBaseDB()
    {
        // 1、create db folder
        string db_path_uri = GetBaseDbFolder();

        // 2、create db header file
        string db_file_uri = GetOrCreateDefaultDbFile(DEFAULT_DB_FOLDER_NAME);

        // 3、write data into db header file
        DB default_db;
        default_db.db_name = DEFAULT_DB_FILE_NAME;
        default_db.db_description = "The base db of this dbms, has a table, which stores all db about this dbms";
        default_db.db_all_tables_path = GetDefaultTablePath(db_path_uri);
        SerializeDBFile(default_db, db_file_uri);
    }

    // for sql : CREATE TABLE db_name FROM table_name
    void CreateTable(string db_name, string table_name)
    {
        // TODO: 
    }
    
    // for sql : CREATE TABLE db_name FROM table_name
    void CreateDefaultTable(string db_name)
    {
        // TODO:
    }

    // create a special default table for base db, this table will store all db's name.
    void CreateDefaultTableForBaseDb()
    {
        // 1、create default table header file
        string tables_path = GetDefaultTablePath(GetBaseDbFolder());
        string default_table_file_uri = GetAndCreateDefaultTableFile(tables_path);

        // 2、write data into table header file (data address is 0, because the data of default table must in the first block of table data file)
        // construct a block to read from or write to disk, construct a column_table(only has one column), and insert to block
        TableBlock block;

        ColumnTable ct;
        ct.table_name = DEFAULT_TABLE_NAME;
        ct.table_type = COMMON;
        ct.InsertColumn(DEFAULT_TABLE_COLUMN_NAME_ONE, VCHAR, VCHAR_LENGTH, NONE, 0);    // insert one column header, column name is db_names

        block.InsertTable(&ct);    // insert table header data to block

        // write back
        TableFileManagement tfmm;
        fstream file_stream;
        tfmm.OpenDataFile(default_table_file_uri, file_stream);    // open table header file, like "test.tvdbb"
        default_address_type free_address = tfmm.GetNewBlockAddress(file_stream);    // get free block address in file
        
        tfmm.WriteBackBlock(file_stream, free_address, block.data);

        // Test deserialize from file: read from file and deserialize, this default block is at 0 offset
        TableBlock new_block;
        tfmm.ReadFromFile(file_stream, 0, new_block.data);
        new_block.DeserializeFromBuffer(new_block.data);
        assert(new_block.table_amount == 1);
        assert(new_block.tables_begin_address[new_block.table_amount - 1] == 4008);
        // cout 
        // << "--------Deserialize table block data--------"
        // << endl
        // << " table_amount: " 
        // << new_block.table_amount 
        // << " free_space: "
        // << new_block.free_space 
        // << " next_block_pointer: "
        // << new_block.next_block_pointer 
        // << "tables_begin_address: "
        // << new_block.tables_begin_address[new_block.table_amount - 1] 
        // << endl
        // << "------Deserialize table block data end------"
        // << endl;
        // Test deserialize ColumnTable from new_block, read the last table
        ColumnTable new_ct;
        new_ct.Deserialize(new_block.data, new_block.tables_begin_address[new_block.table_amount - 1]);
        assert(new_ct.column_size == 1);
        assert(new_ct.column_name_array[new_ct.column_size - 1] == DEFAULT_TABLE_COLUMN_NAME_ONE);
        assert(new_ct.column_storage_address_array[new_ct.column_size - 1] == 0);
        // cout 
        // << "-----Deserialize table header data-----"
        // << endl
        // << "column_size : " 
        // << new_ct.column_size
        // << " column_name_array: "
        // << new_ct.column_name_array[new_ct.column_size - 1]
        // << " column_storage_address_array: "
        // << new_ct.column_storage_address_array[new_ct.column_size - 1]
        // << endl
        // << "--------Deserialize table block data--------"
        // << endl;
    }


    // for sql: INSERT INTO table_name(column_name) VALUES (data);
    void InsertIntoTable(string db_name, string table_name, string column_name, char* data)
    {
        // // open db file, like "db.tvdbb", and deserialize to get default table name
        // string db_file_name = GetOrCreateDbFolder(db_name) + db_name + DB_FILE_SUFFIX;
        // DB db_file;
        // DeserializeDBFile(db_file, db_file_name);

        // // open default table file, like "default_table.tvdbb"
        // TableBlock block;
        // TableFileManagement tfmm;
        // fstream table_header_file_stream;
        // tfmm.OpenDataFile(db_file.db_all_tables_path, table_header_file_stream); // open file
        // tfmm.ReadFromFile(table_header_file_stream, 0, block.data);  // read a block
        // block.DeserializeFromBuffer(block.data); // deserialize data

        // // deserialize the first table in tables block
        // ColumnTable default_table;
        // default_table.Deserialize(block.data, block.tables_begin_address[0]);

        // 
    }

    // check table exist in db, if not exist, then throw error. Or return it.
    ColumnTable CheckTableExist(string db_name, string table_name)
    {
        // open db file, like "db.tvdbb", and deserialize to get default table name
        string db_file_name = GetOrCreateDbFolder(db_name) + db_name + DB_FILE_SUFFIX;
        DB db_file;
        DeserializeDBFile(db_file, db_file_name);

        // open default table file, like "default_table.tvdbb"
        TableBlock block;
        TableFileManagement tfmm;
        fstream table_header_file_stream;
        tfmm.OpenDataFile(db_file.db_all_tables_path, table_header_file_stream); // open file
        tfmm.ReadFromFile(table_header_file_stream, 0, block.data);  // read a block
        block.DeserializeFromBuffer(block.data); // deserialize data

        // deserialize the first table in tables block
        for (default_amount_type i = 0; i < block.table_amount; i++)
        {
            ColumnTable table;
            table.Deserialize(block.data, block.tables_begin_address[i]);
            if (table.table_name == table_name)
            {
                return table;
            }
        }
        
        throw std::runtime_error("Can not find table" + table_name + "in db " + db_name);
    }

    bool CheckColumnExist()
    {
        
    }

public:
    DBManagement()
    {
        file_mm = new FileManagement();
    }

    // create the first db of this system, all information about this db will be stored in this db.
    // its name and position has been configed in config.h
    void CreateDefaultDB() 
    {
        
        /*
            CREATE DATABASE base_db;

            CREATE TABLE default_table (
                db_names VCHAR(20)
            )
            FROM base_db;
        */

        // CREATE DATABASE base_db;


        // // create default db folder
        // string db_folder = GetBaseDbFolder();

        // // create db file, db file has the same name as db folder
        // string db_file_name = db_folder + "/" + DEFAULT_DB_FILE_NAME;
        // db_file_name = db_file_name + DB_FILE_SUFFIX;
        // file_mm->ReadOrCreateFile(db_file_name).close();

        // // // create table folder
        // // string default_table_folder = db_folder + "/" + DEFAULT_TABLE_FOLDER;
        // // file_mm->OpencvDirAndMkdir(default_table_folder);

        // // // create table file, one db only has one table file, which contains all table blocks.
        // // string default_table_uri = default_table_folder + "/" + DEFAULT_TABLE_NAME; 
        // // default_table_uri = default_table_uri + TABLE_FILE_SUFFIX;
        // // file_mm->ReadOrCreateFile(default_table_uri).close();

        // // store db file
        // DB default_db;
        // default_db.db_name = DEFAULT_DB_FILE_NAME;
        // default_db.db_description = "The base db of this dbms, has a table, which stores all db about this dbms";
        // default_db.db_all_tables_path = GetDefaultTablePath(db_folder);
        // SerializeDBFile(default_db, db_file_name);

        // // test DeserializeDBFile
        // DB new_db;
        // DeserializeDBFile(new_db, db_file_name);
        // std::cout << "default_db description: " << new_db.db_name << "  " << new_db.db_description << "  " << new_db.db_all_tables_path << std::endl;

        // // create default table
        // string base_db_folder = GetBaseDbFolder();
        // CreateDefaultTable(base_db_folder, DEFAULT_TABLE_COLUMN_NAME_ONE);
    }




    /**
     * @brief Cal and return the default table data path base on the base db path.
     * @return default table data path of the input db path.
     */
    string GetDefaultTableDataFolderOfBaseDbPath()
    {   
        string db_folder = GetBaseDbFolder();
        string table_path = GetDefaultTablePath(db_folder);

        // create table data folder
        string default_table_data_folder = table_path + "/" + DEFAULT_TABLE_DATA_FOLDER;
        file_mm->OpencvDirAndMkdir(default_table_data_folder);
        return default_table_data_folder;
    }
    
    /**
     * @brief Cal and return the default table data path base on the input db path.
     * @param table_path db folder path
     * @return default table data folder path of the input db path.
     */
    string GetAndCreateDataFolderPath(string& db_path)
    {   
        string table_path = GetDefaultTablePath(db_path);
        string default_table_data_folder = table_path + "/" + DEFAULT_TABLE_DATA_FOLDER;
        file_mm->OpencvDirAndMkdir(default_table_data_folder);
        return default_table_data_folder;
    }

    /**
     * @brief Get the data file path of input table_name. if not exist, then create it.
     * @param data_folder_path data file store folder
     * @param table_name table name
     * @return data file path of input table_name
     */
    string GetAndCreateDataFile(string& data_folder_path, string& table_name)
    {
        string table_data_file = data_folder_path;
        table_data_file += "/";
        table_data_file += table_name;
        table_data_file += TABLE_DATA_FILE_SUFFIX;

        return table_data_file;
    }

    void CreateDefaultTableOfBaseDb()
    {
        string base_db_path = GetBaseDbFolder();
        // CreateDefaultTable(base_db_path, DEFAULT_TABLE_COLUMN_NAME_ONE);
    }

    // unused
    // create the default table of base db, includes: table header file(), tables data folder, table data file, default row data
    // this table will stores all of the else db names. so can check whether are they existed in this system.
    // void CreateDefaultTable(string& db_path, string column_name)
    // {
    //     string table_header_file_uri = GetDefaultTablePath(db_path);
    //     string table_data_folder_uri = GetDefaultTableDataFolderPath(db_path);
    //     string table_name = DEFAULT_TABLE_DATA_FILE_NAME;
    //     string table_data_file_uri = GetAndCreateTableDataFile(table_data_folder_uri, table_name);
         

    //     cout << "Create table: " << table_header_file_uri << endl;
    //     cout << "Create data folder: " << table_data_folder_uri << endl;
    //     cout << "Create data file of table : table_name" << table_data_file_uri << endl;
        
    //     // construct a block
    //     TableBlock block;

    //     // construct a column_table
    //     ColumnTable ct;
    //     ct.table_name = DEFAULT_TABLE_NAME;
    //     ct.table_type = COMMON;
        
    //     // insert one column header
    //     ct.InsertColumn(column_name, VCHAR, VCHAR_LENGTH, NONE, NONE);

    //     // update block data
    //     block.InsertTable(&ct);

    //     // write data in memory back to file, use TableFileManagement
    //     TableFileManagement tfmm;
    //     fstream file_stream;

    //     // open table header file, like "test.tvdbb"
    //     tfmm.OpenDataFile(table_header_file_uri, file_stream);

    //     // get free block address in file
    //     default_address_type free_address = tfmm.GetNewBlockAddress(file_stream);
        
    //     // write back
    //     cout << "begin write back" << endl;
    //     tfmm.WriteBackBlock(file_stream, free_address, block.data);
    //     cout << "end write back" << endl;

    //     // Test deserialize from file: read from file and deserialize, this default block is at 0 offset
    //     TableBlock new_block;
    //     tfmm.ReadFromFile(file_stream, 0, new_block.data);
    //     new_block.DeserializeFromBuffer(new_block.data);

    //     cout 
    //     << "Deserialize table block data:" 
    //     << new_block.table_amount 
    //     << " free_space: "
    //     << new_block.free_space 
    //     << " next_block_pointer: "
    //     << new_block.next_block_pointer 
    //     << "tables_begin_address: "
    //     << new_block.tables_begin_address 
    //     << endl;

    //     // Test deserialize ColumnTable from new_block, read the last table
    //     ColumnTable new_ct;
    //     new_ct.Deserialize(new_block.data, new_block.tables_begin_address[new_block.table_amount - 1]);
    //     cout 
    //     << "Deserialize table header data: column_size : " 
    //     << new_ct.column_size
    //     << " column_name_array: "
    //     << new_ct.column_name_array[new_ct.column_size - 1]
    //     << " column_storage_address_array: "
    //     << new_ct.column_storage_address_array[new_ct.column_size - 1]
    //     << ""
    //     << endl;
    //     // cout << "end read" << endl;



    //     file_stream.close();
    // }

    void CreateBaseTable()
    {

    }

    void InsertDefaultDBDataIntoBaseTable()
    {
        // construct row data 
        string row_data = DEFAULT_DB_FILE_NAME;
        char* row_data_array = new char[VCHAR_LENGTH];
        memcpy(row_data_array, &row_data, row_data.length());
    };
        

    // store a db object to file
    void SerializeDBFile(DB& db, string file_path)
    {   
        char endl = '\n';
        file_mm->WriteFile(file_path, db.db_name.c_str(), db.db_name.length());
        file_mm->WriteFileAppend(file_path, &endl, 1);
        file_mm->WriteFileAppend(file_path, db.db_description.c_str(), db.db_description.length());
        file_mm->WriteFileAppend(file_path, &endl, 1);
        file_mm->WriteFileAppend(file_path, db.db_all_tables_path.c_str(), db.db_all_tables_path.length());
        file_mm->WriteFileAppend(file_path, &endl, 1);
        cout << " write end " << endl;
    }
    
    // load a db object from filef
    void DeserializeDBFile(DB& db, string file_path)  
    {  
        std::ifstream file_read = file_mm->ReadOrCreateFile(file_path);
        getline(file_read, db.db_name);
        getline(file_read, db.db_description);
        getline(file_read, db.db_all_tables_path);
        file_read.close();
    } 

};

}

#endif // VDBMS_BENCH_DB_MANAGEMENT_H_