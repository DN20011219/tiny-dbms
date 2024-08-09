// Copyright (c) 2024 by dingning
//
// file  : bd.h
// since : 2024-07-18
// desc  : TODO.

#ifndef VDBMS_META_DB_DB_H_
#define VDBMS_META_DB_DB_H_

#include <string>
#include <vector>

#include "../table/column_table.h"

using std::string;
using std::vector;

namespace tiny_v_dbms {

class DB
{

public:
    string db_name;
    string db_description;
    string db_all_tables_path; // the first table block of this db. so can find all tables from it. here use the url for first addressing.  
    
    vector<ColumnTable> tables;

    DB() = default;

    DB(const DB& db)
    {
        db_name = db.db_name;
        db_description = db.db_description;
        db_all_tables_path = db.db_all_tables_path;

        tables.resize(db.tables.size());
        for (int i = 0; i < db.tables.size(); i++)
        {
            tables[i] = tables[i];
        }
    }

    // store a db object to string
    string SerializeDBFile()
    {   

        string data = "";
        data += db_name + "\n";
        data += db_description + "\n";
        data += db_all_tables_path;
        
        return data;
    }
    
    // load a db object from string
    void DeserializeDBFile(char* data, default_length_size data_length)  
    {  
        db_name = "";
        db_description = "";
        db_all_tables_path = "";

        default_length_size data_pointer = 0;

        while (data_pointer < data_length)
        {
            if (data[data_pointer] == '\n')
            {
                data_pointer++;
                break;
            }   
            db_name += data[data_pointer];
            data_pointer++;
        }

        while (data_pointer < data_length)
        {
            if (data[data_pointer] == '\n')
            {
                data_pointer++;
                break;
            }   
            db_description += data[data_pointer];
            data_pointer++;
        }

        while (data_pointer < data_length)
        { 
            db_all_tables_path += data[data_pointer];
            data_pointer++;
        }

    } 

};


}

#endif // VDBMS_META_DB_DB_H_