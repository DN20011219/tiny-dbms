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
    void GetInstallPath(string& install_path) 
    {
        string cache_file_uri = INSTALL_PATH_CACHE_FILE;
        std::ifstream file_read = file_mm->ReadOrCreateFile(cache_file_uri);
        getline(file_read, install_path);
        file_read.close();
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

        string install_path;
        GetInstallPath(install_path);

        // create default db folder
        string db_folder = install_path + "/" + DEFAULT_DB_FOLDER_NAME;
        file_mm->OpencvDirAndMkdir(db_folder);

        // create db file, db file has the same name as db folder
        string db_file_name = db_folder + "/" + DEFAULT_DB_FILE_NAME;
        db_file_name = db_file_name + DB_FILE_SUFFIX;
        file_mm->ReadOrCreateFile(db_file_name).close();

        // // create table folder
        // string default_table_folder = db_folder + "/" + DEFAULT_TABLE_FOLDER;
        // file_mm->OpencvDirAndMkdir(default_table_folder);

        // // create table file, one db only has one table file, which contains all table blocks.
        // string default_table_uri = default_table_folder + "/" + DEFAULT_TABLE_NAME; 
        // default_table_uri = default_table_uri + TABLE_FILE_SUFFIX;
        // file_mm->ReadOrCreateFile(default_table_uri).close();

        // store db file
        DB default_db;
        default_db.db_name = DEFAULT_DB_FILE_NAME;
        default_db.db_description = "The base db of this dbms, has a table, which stores all db about this dbms";
        default_db.db_all_tables_path = GetDefaultTablePath(db_folder);
        SerializeDBFile(default_db, db_file_name);

        // test DeserializeDBFile
        DB new_db;
        DeserializeDBFile(new_db, db_file_name);
        std::cout << "default_db description: " << new_db.db_name << "  " << new_db.db_description << "  " << new_db.db_all_tables_path << std::endl;

        // create default table
        CreateDefaultTable(new_db.db_all_tables_path);
    }


    /**
     * @brief Cal and return the default table path base on the input db path.
     * @param db_path db folder path
     * @return default table path of the input db path.
     */
    string GetDefaultTablePath(string& db_path)
    {   
        // create table folder
        string default_table_folder = db_path + "/" + DEFAULT_TABLE_FOLDER;
        file_mm->OpencvDirAndMkdir(default_table_folder);
        return default_table_folder;
    }

    /**
     * @brief Cal and return the default table data path base on the input table path.
     * @param table_path table folder path
     * @return default table data path of the input db path.
     */
    string GetDefaultTableDataPath(string& table_path)
    {   
        // create table data folder
        string default_table_data_folder = table_path + "/" + DEFAULT_TABLE_DATA_FOLDER;
        file_mm->OpencvDirAndMkdir(default_table_data_folder);
        return default_table_data_folder;
    }

    string CreateTableDataFile(string& data_folder_path, string& table_name)
    {
        string table_data_file = data_folder_path;
        table_data_file += "/";
        table_data_file += table_name;
        table_data_file += TABLE_DATA_FILE_SUFFIX;

        return table_data_file;
    }

    // create the first table of first db.
    // this table will stores all of the else db names. so can check whether are they existed in this system.
    void CreateDefaultTable(string& tables_path)
    {
        string table_file = tables_path;
        table_file += "/";
        table_file += DEFAULT_TABLE_NAME;
        table_file += TABLE_FILE_SUFFIX;

        cout << "CreateDefaultTable: " << table_file << endl;

        string table_data_uri = GetDefaultTableDataPath(tables_path);
        cout << "Create data folder: " << table_data_uri << endl;

        string table_name = DEFAULT_TABLE_DATA_FILE_NAME;
        string data_file_uri = CreateTableDataFile(tables_path, table_name);
        
        cout << "Create data file: " << data_file_uri << endl;
        
        // construct a block
        TableBlock block;

        // construct a column_table
        ColumnTable ct;
        ct.table_name = DEFAULT_TABLE_NAME;
        ct.table_type = COMMON;
        
        // construct 
        ct.InsertColumn(DEFAULT_TABLE_COLUMN_NAME_ONE, VCHAR, VCHAR_LENGTH, NONE, NONE);

        // update block data
        block.InsertTable(&ct);

        // write data in memory back to file, use TableFileManagement
        TableFileManagement tfmm;
        fstream file_stream;

        // open table file, like "test.tvdbb"
        tfmm.OpenDataFile(table_file, file_stream);

        // get free block address in file
        default_address_type free_address = tfmm.GetNewBlockAddress(file_stream);
        
        // write back
        cout << "begin write back" << endl;
        tfmm.WriteBackBlock(file_stream, free_address, block.data);
        cout << "end write back" << endl;

        // Test deserialize from file: read from file and deserialize, this default block is at 0 offset
        TableBlock new_block;
        cout << "begin read" << endl;
        tfmm.ReadFromFile(file_stream, 0, new_block.data);
        new_block.DeserializeFromBuffer(new_block.data);
        cout << "end read" << endl;

        cout 
        << "Deserialize table data:" 
        << new_block.table_amount 
        << " free_space: "
        << new_block.free_space 
        << " next_block_pointer: "
        << new_block.next_block_pointer 
        << "tables_begin_address: "
        << new_block.tables_begin_address 
        << endl;

        file_stream.close();
    }

    void CreateDefaultTableData()
    {
        
    }

    // TODO
    void CreateTable(string& table_name, string& db_name)
    {
        // check it areadly existed

        // 
    }

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
    }
    
    // load a db object from file
    void DeserializeDBFile(DB& db, string file_path)  
    {  
        std::ifstream file_read = file_mm->ReadOrCreateFile(file_path);
        getline(file_read, db.db_name);
        getline(file_read, db.db_description);
        getline(file_read, db.db_all_tables_path);
        file_read.close();
    } 

    // string SerializeTableFile(ColumnTable& table)
    // {  


    // }


};

}

#endif // VDBMS_BENCH_DB_MANAGEMENT_H_