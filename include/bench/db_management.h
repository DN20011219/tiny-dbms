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
#include "../meta/table/column_table.h"

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
        FileManagement mm;
        file_mm = &mm;
    }

    void CreateDefaultDB() {

        string install_path;
        GetInstallPath(install_path);

        // create default db folder
        string db_folder = install_path + "/" + DEFAULT_DB_FOLDER_NAME;
        file_mm->OpencvDirAndMkdir(db_folder);

        // create db file, db file has the same name as db folder
        string db_file_name = db_folder + "/" + DEFAULT_DB_FILE_NAME;
        db_file_name = db_file_name + DB_FILE_SUFFIX;
        file_mm->ReadOrCreateFile(db_file_name).close();

        // create table folder
        string default_table_folder = db_folder + "/" + DEFAULT_TABLE_FOLDER;
        file_mm->OpencvDirAndMkdir(default_table_folder);

        // create table file, one db only has one table file, which contains all table blocks.
        string default_table_uri = default_table_folder + "/" + DEFAULT_TABLE_NAME; 
        default_table_uri = default_table_uri + TABLE_FILE_SUFFIX;
        file_mm->ReadOrCreateFile(default_table_uri).close();

        // store db file
        DB default_db;
        default_db.db_name = DEFAULT_DB_FILE_NAME;
        default_db.db_description = "The base db of this dbms, has a table, which stores all db about this dbms";
        default_db.db_header_path = default_table_uri;
        SerializeDBFile(default_db, db_file_name);
        std::cout << "name ----" << default_db.db_name << std::endl;

        // test DeserializeDBFile
        DB new_db;
        DeserializeDBFile(new_db, db_file_name);
        std::cout << "default_db description: " << new_db.db_name << " " << new_db.db_description << " " << new_db.db_header_path << std::endl;

        // store table file, this table contains all db names in this db management
        Column_Table default_table;
        default_table.table_name = DEFAULT_TABLE_NAME;
        // SerializeTableFile(default_table, default_table_uri);

       
    }

    // store a db object to file
    void SerializeDBFile(DB& db, string file_path)
    {   
        char endl = '\n';
        file_mm->WriteFile(file_path, db.db_name.c_str(), db.db_name.length());
        file_mm->WriteFileAppend(file_path, &endl, 1);
        file_mm->WriteFileAppend(file_path, db.db_description.c_str(), db.db_description.length());
        file_mm->WriteFileAppend(file_path, &endl, 1);
        file_mm->WriteFileAppend(file_path, db.db_header_path.c_str(), db.db_header_path.length());
        file_mm->WriteFileAppend(file_path, &endl, 1);
    }
    
    // load a db object from file
    void DeserializeDBFile(DB& db, string file_path)  
    {  
        std::ifstream file_read = file_mm->ReadOrCreateFile(file_path);
        getline(file_read, db.db_name);
        getline(file_read, db.db_description);
        getline(file_read, db.db_header_path);
        file_read.close();
    } 

    string SerializeTableFile(Column_Table& table)
    {  


    }

    void SerializeTableBlock(Table_Block& )
    {

    }

};

}

#endif // VDBMS_BENCH_DB_MANAGEMENT_H_