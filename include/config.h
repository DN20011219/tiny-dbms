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

    /*
    || means it is a folder, string means it is a file
    folder struct descripition: Each db has one folder named as its name, adn all data is stored in this folder.
    Each db has one default table, this table will store all tables name in this db. But in the default db, this table will store all the dbs name.

    |root|
    --|base_db|
    ----base_db.tvdb
    ----|tables|
    ------default_table.tvdbb (store the table header)
    ------|data|
    --------default_table.data (contains all data of default_table table)
    */
    #define INSTALL_PATH_CACHE_FILE "./install_url.txt"         // the uri of config file, use to store the path where installed this dbms

    #define DEFAULT_DB_FOLDER_NAME "base_db"                 // the name of installed default db
    #define DEFAULT_DB_FILE_NAME "base_db"                  // the name of default db file
    #define DB_FILE_SUFFIX ".tvdb"                             // the suffix of all db file

    #define DEFAULT_TABLE_FOLDER "tables"                       // this folder will store all data about tables , it is under "db_name"(DB_FOLDER_NAME) folder
    #define DEFAULT_TABLE_NAME "default_table"                 // the name of default db table file, each db has one default_table to store all tables header information in db
    #define TABLE_FILE_SUFFIX ".tvdbb"                         // the suffix of table header file
   
    #define DEFAULT_TABLE_COLUMN_NAME_ONE "db_names"            // the default column name of default table, this table only has one column, to store all db name sin this system
    #define DEFAULT_TABLE_COLUMN_NAME_TWO "tables_names"         // the default column name of default table, used in user's db.

    #define DEFAULT_TABLE_DATA_FOLDER "data"                  // this folder will store all exact data of this db, it is under "tables"(DEFAULT_TABLE_FOLDER) folder
    #define DEFAULT_TABLE_DATA_FILE_NAME "default_table"        // the name of default db table data file
    #define TABLE_DATA_FILE_SUFFIX ".data"                      // the suffix of table data file

    #define DEFAULT_TABLE_LOG_FOLDER "log"
    #define DEFAULT_TABLE_LOG_FILE_NAME "default_table"
    #define TABLE_LOG_FILE_SUFFIX ".log"  

    // config about storage
    #define MEMORY_SIZE 10737418239 / 4             // the size of the memory, any memory using need to acquire space here, 10737418239 byte (1g) / 4 = 256 mb
    #define SLOT_AMOUNT 10737418239 / 4 / 4096      // the amount of slots on buffer pool
    #define BLOCK_SIZE 4096                         // the size of one block is 4096 byte (4kb)
    #define LOG_MANAGER_INSRANCE_AMOUNT 4096        // the log manager amount, it should as same as block amout in memory_management


    // config about meta data toe
    #define BASE_VECTOR Vector<int, double>     // basic vector, LENGTH_TYPE is int, DATA_TYPE is double

    // config about default data type
    #define default_length_size int             // use int as the default type of length.
    #define default_amount_type int             // use int as the default type of item amount.
    #define default_enum_type int               // use int to means enum item.
    #define default_address_type int            // use int as the default type to store address.
    #define default_pointer_size 4              // use 32bit as the default size of pointer, this may be need to change for adaption to 64bit device
    #define default_long_int size_t             // use size_t as the long int type

    // config about enum data
    enum TABLE_TYPE {DEFAULT, COMMON};          // type of table, now no use
    enum column_type {NUMBER, VECTOR, VCHAR};
    // enum column_length {NUMBER_LENGTH = 4, VCHAR_LENGTH = 50};
    #define VCHAR_LENGTH 50
    #define INT_LENGTH sizeof(int)
    #define FLOAT_LENGTH sizeof(float)
    #define RAW_LENGTH 50

    enum column_index_type {NONE, FLAT};

/*---- check platform type, now support win & mac mainly ----*/ 
#if defined(__APPLE__) && defined(__MACH__)
    #ifndef PLATFORM_IS_MAC
    #define PLATFORM_IS_MAC
    #endif // PLATFORM_IS_MAC
#elif defined(_WIN32) || defined(_WIN64)
    #ifndef PLATFORM_IS_WIN
    #define PLATFORM_IS_WIN
    #endif // PLATFORM_IS_WIN
#else
    #error "Unsupported platform"
#endif
/*-------------------------------------------------------------*/


/*---- config about client and server ----*/

    // config about msg body, which is used to communication between client and server
    #define MSG_DATA_LENGTH 128                 
    #define IP_LENGTH 15
    #define PORT_LENGTH 4
    #define IDENTITY_LENGTH 4
    #define CONNECT_DB_NAME_LENGTH_LENGTH 4
    #define CONNECT_DB_NAME_LENGTH  100

    #define WORK_MSG_DATA_LENGTH 2000            // config about msg used by worker, here limit the max length is 2000
    #define SQL_MAX_LENGTH 1996             
    #define SQL_LENGTH_LENGTH 4

// use msg queue on mac os to communicate.
#if defined(PLATFORM_IS_MAC)
    // macOS-specific includes
    #include <sys/ipc.h>
    #include <sys/msg.h>

    #define CONNECTOR_MESSAGE_KEY ftok("tvdbms_connect", 7) 
    #define WORKER_MESSAGE_KEY ftok("tvdbms_work", 8) 
    #define BASE_DATABSE_MESSAGE_KEY ftok("tvdbms_base_db_work", 9) 
    #define CONNECTOR_MESSAGE_ID 40286          // this id is used to confirm the msg queue set id used by connecter and clients
    #define WORKER_MESSAGE_ID 40287             // this id is used to confirm the msg queue set id used by workers and clients

    #define BASE_DB_WORKER_RECEIVE_QUEUE_ID 875

    #ifndef CONNECTOR_MSG_KEY
    #define CONNECTOR_MSG_KEY
    const int connector_msg_key = msgget(ftok("/tmp/tvdbms/tvdbms_connect", 1), IPC_CREAT | 0755);
    #endif // CONNECTOR_MSG_KEY

    #ifndef WORKER_MSG_KEY
    #define WORKER_MSG_KEY
    const int worker_msg_key = msgget(ftok("/tmp/tvdbms/tvdbms_work", 2), IPC_CREAT | 0755);
    #endif // WORKER_MSG_KEY
    
    #ifndef BASE_DATABASE_WORKER_MSG_KEY
    #define BASE_DATABASE_WORKER_MSG_KEY
    const int base_db_worker_msg_key = msgget(ftok("/tmp/tvdbms/tvdbms_base_db_work", 3), IPC_CREAT | 0755);
    #endif // BASE_DATABASE_WORKER_MSG_KEY

    #ifndef FREE_QUEUE_ID_ARRAY
    #define FREE_QUEUE_ID_ARRAY
    const long free_queue_id_array[] = {1, 3, 5, 7, 9};
    #endif // FREE_QUEUE_ID_ARRAY

#endif

// use named pipes on win64 to communicate.
#if defined(PLATFORM_IS_WIN)

    #define MAX_PIPE_CONNECTIONS 10                                                     // 定义最大管道数为10，用于控制并发数


    /*win平台对管道通过两种方式进行标识，其中最主要最简单的就是管道名*/
    const std::string ROOT_PIPE_NAME = "\\\\.\\pipe\\";                                 // 这里采用“.”，也就是本机，同时作为服务器与客户端运行的根通道名
    const std::string BASE_DB_WORKER_REC_PIPE_NAME = "server_base_db_worker_rec";       // base db worker通过这个name标识的通道接收请求
    const std::string CONNECTOR_RECEIVE_PIPE_NAME = "connector_rec";                    // connector通过监听这个通道，处理来自不同客户端的连接请求，请求体无意义，主要作用是唤醒connector的监听线程，拿到一个唯一的用于后续进行沟通的通道名
    const std::string WORKER_PIPE_NAME_SUFFIX = "pipe_";                                // worker通道前缀名，用于worker通道接收和发送消息

    // 对pipename进行拼接处理的工具函数
    std::string SplicePipeName(std::string name)
    {
        return ROOT_PIPE_NAME + name;
    }
    #define PIPE_NAME_TEXT(name) TEXT(SplicePipeName(name).c_str())

#endif

/*---------------------------------------*/


}

#endif // VDBMS_CONFIG_H_