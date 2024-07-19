// Copyright (c) 2024 by dingning
//
// file  : config.h
// since : 2024-07-16
// desc  : Basic config of this project, contains the default vector type ...

#ifndef VDBMS_CONFIG_H_
#define VDBMS_CONFIG_H_

#include "meta/vector.h"

namespace tiny_v_dbms {
    // config about this dbms
    #define INSTALL_PATH_CACHE_FILE "./install_url.txt";         // the uri of config file, use to store the path where installed this dbms
    #define DEFAULT_DB_FOLDER_NAME "default_db";                 // the name of installed default db folder
    #define DB_FILE_SUFFIX ".tvdb";                              // the suffix of all db file
    #define DEFAULT_DB_FILE_NAME "default_db";                   // the name of default db file
    #define TABLE_FILE_SUFFIX ".tvdbb";                          // the suffix of all table file
    #define DEFAULT_TABLE_FOLDER "tables";                       // this folder will store all tables data, it is under "db_name"(DB_FOLDER_NAME) folder
    #define DEFAULT_TABLE_NAME "default_table";                  // the name of default db table file, each db has one default_table to store all tables information in db

    // config about storage
    #define MEMORY_SIZE 10737418239;            // the size of the memory, any memory using need to acquire space here, 10737418239 byte (1g)
    #define BLOCK_SIZE 4096;                    // the size of one block is 4096 byte (4kb)

    // config about meta data toe
    #define BASE_VECTOR Vector<int, double>     // basic vector, LENGTH_TYPE is int, DATA_TYPE is double

    // config about default data type
    #define default_length_size int             // use int as the default type of length.
    #define default_amount_type int             // use int as the default type of item amount.
    #define default_enum_type int               // use int to means enum item.
    #define default_address_type int            // use int as the default type to store address.
    #define default_pointer_size 4              // use 32bit as the default size of pointer, this may be need to change for adaption to 64bit device

    // config about enum data
    enum column_type {NUMBER, VECTOR, VCHAR};
    enum column_index_type {NONE, FLAT};
}

#endif // VDBMS_CONFIG_H_