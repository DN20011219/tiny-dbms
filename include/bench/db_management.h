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
        if (!file_read) {
            throw std::runtime_error("Failed to open install file for reading");
        }
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
        file_mm->ReadOrCreateFile(db_file_name);

        // create table folder
        string default_table_folder = db_folder + "/" + DEFAULT_TABLE_FOLDER;
        file_mm->OpencvDirAndMkdir(default_table_folder);

        // create table file, one db only has one table file, which contains all table blocks.
        string default_table_uri = default_table_folder + "/" + DEFAULT_TABLE_NAME; 
        default_table_uri = default_table_uri + TABLE_FILE_SUFFIX;
        file_mm->ReadOrCreateFile(default_table_uri);

        // store db file
        DB default_db;
        default_db.db_name = DEFAULT_DB_FILE_NAME;
        default_db.db_description = "The base db of this dbms, has a table, which stores all db about this dbms";
        default_db.db_header_path = default_table_uri;
        SerializeDBFile(default_db, db_file_name);

        // store table file, this table contains all db names in this db management
        Column_Table default_table;
        default_table.table_name = DEFAULT_TABLE_NAME;
        SerializeTableFile(default_table, default_table_uri);
    }


    void SerializeDBFile(DB& db, string file_path)
    {
        file_mm->WriteFile(file_path, (char*) &db.db_name, db.db_name.length());
        file_mm->WriteFile(file_path, (char*) &db.db_description, db.db_description.length());
        file_mm->WriteFile(file_path, (char*) &db.db_header_path, db.db_header_path.length());
    }
    
    void DeserializeDBFile()  
    {  
        
    } 

    void SerializeTableFile(Column_Table& table, string file_path)
    {

    }

};

}

#endif // VDBMS_BENCH_DB_MANAGEMENT_H_