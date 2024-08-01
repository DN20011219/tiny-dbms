// Copyright (c) 2024 by dingning
//
// file  : log.h
// since : 2024-07-29
// desc  : this is the definition of log unit. Each table has it's own log file, 
// and each change (updata, delete) of record will firstly write to the tail of the log file. 
// Then dbms will find some empty time to write the record in log file back to table data blocks.
// So can decrease the cost that flash total block data to disk, even only change one bit.


#ifndef VDBMS_META_LOG_H_
#define VDBMS_META_LOG_H_

#include <fstream>
#include <mutex>

#include "./block/data_block.h"
#include "../storage/file_management.h"
#include "../storage/block_file_management.h"
#include "../config.h"

using std::fstream;
using std::mutex;


namespace tiny_v_dbms {

enum LogType { UPDATE_LOG, DELETE_LOG };

class LogUnitHeader
{

public:
    LogType log_type;
    default_length_size log_data_length; // length of log data body, if the log_type is DELETE, then log_data_length is 0
    default_address_type block_address;
    default_address_type record_address; 

    default_length_size GetHeaderLength()
    {
        return sizeof(LogType) + sizeof(default_length_size) + 2 * sizeof(default_address_type);
    }

    void Serialize(char* serialize_data)
    {
        default_address_type offset = 0;
        
        memcpy(serialize_data + offset, &log_type, sizeof(LogType));
        offset += sizeof(LogType);

        if (log_type == DELETE_LOG)
        {
            // no need to serialize the log_data_length, must be 0
        }
        else
        {
            memcpy(serialize_data + offset, &log_data_length, sizeof(default_length_size));
            offset += sizeof(default_length_size);
        }

        memcpy(serialize_data + offset, &block_address, sizeof(default_address_type));
        offset += sizeof(default_address_type);

        memcpy(serialize_data + offset, &record_address, sizeof(default_address_type));
    }

    void Deserialize(char* serialize_data)
    {
        default_address_type offset = 0;
        
        memcpy(&log_type, serialize_data + offset, sizeof(LogType));
        offset += sizeof(LogType);

        if (log_type == DELETE_LOG)
        {
            log_data_length = 0;
        }
        else
        {
            memcpy(&log_data_length, serialize_data + offset, sizeof(default_length_size));
            offset += sizeof(default_length_size);
        }

        memcpy(&block_address, serialize_data + offset, sizeof(default_address_type));
        offset += sizeof(default_address_type);

        memcpy(&record_address, serialize_data + offset, sizeof(default_address_type));
    }
};

class LogUnit
{

public:

    LogUnitHeader header;   // record the struct data of log
    char* log_body;     // record the data of log.

    ~LogUnit()
    {
        if (header.log_data_length > 0)
            delete[] log_body;
    }

    default_length_size GetLength()
    {
        return header.GetHeaderLength() + header.log_data_length;
    }

    void Serialize(char* buffer)
    {
        buffer = new char[header.GetHeaderLength() + header.log_data_length];
        header.Serialize(buffer);
        memcpy(buffer + header.GetHeaderLength(), log_body, header.log_data_length); 
    }

};

// designed to manage the read / write / clean / serialize / deserialize of the log file
// one log manager only controll one table's log file
class LogManager
{
    
private:
    fstream log_file_stream;
    fstream data_file_stream;
    mutex read_write_mutex;

    FileManagement* file_mm;
    BlockFileManagement* bfmm;

    string db_name_store;
    string table_name_store;

public:

    LogManager(string db_name, string table_name)
    {   
        file_mm = new FileManagement();
        bfmm = new BlockFileManagement();

        db_name_store = db_name;
        table_name_store = table_name;

        OpenLogFile();
    }

    ~LogManager()
    {
        if (log_file_stream)
        {
            log_file_stream.close();
        }
        if (data_file_stream)
        {
            data_file_stream.close();
        }
    }

private:    // follows are functions about file path

    // get install path from file
    string GetInstallPath() 
    {
        string install_path;

        std::ifstream file_read = file_mm->ReadOrCreateFile(INSTALL_PATH_CACHE_FILE);
        getline(file_read, install_path);
        file_read.close();

        return install_path;
    }

    // such as install_path/db_name/tables/log
    string CalLogFolderPath(string db_name)
    {
        return GetInstallPath() + "/" + db_name + "/" + DEFAULT_TABLE_FOLDER + "/" + DEFAULT_TABLE_LOG_FOLDER;
    }

    // such as install_path/db_name/tables/log/table_name.log
    string CalLogFilePath(string db_name, string table_name)
    {
        return CalLogFolderPath(db_name) + "/" + table_name + TABLE_LOG_FILE_SUFFIX;
    }

    // such as install_path/db_name/tables/data/table_name.data
    string CalTableDataFilePath(string db_name, string table_name)
    {
        return GetInstallPath() + "/" + db_name + "/" + DEFAULT_TABLE_FOLDER + "/" + DEFAULT_TABLE_DATA_FOLDER + "/" + table_name + TABLE_DATA_FILE_SUFFIX;
    }

    void OpenLogFile()
    {
        string log_file_uri = CalLogFilePath(db_name_store, table_name_store);
        log_file_stream.open(log_file_uri, std::ios::in | std::ios::out);
        if (!log_file_stream)
        {
            file_mm->ReadOrCreateFile(log_file_uri).close();
            log_file_stream.open(log_file_uri, std::ios::in | std::ios::out);
        }
        if (!log_file_stream)
        {
            throw std::runtime_error("LogManager created false, can not open or create log file: " + log_file_uri);
        }
    }

    void OpenTableDataFile()
    {
        string data_file_uri = CalTableDataFilePath(db_name_store, table_name_store);
        data_file_stream.open(data_file_uri, std::ios::in | std::ios::out);
        if (!data_file_stream)
        {
            file_mm->ReadOrCreateFile(data_file_uri).close();
            data_file_stream.open(data_file_uri, std::ios::in | std::ios::out);
        }
        if (!data_file_stream)
        {
            throw std::runtime_error("LogManager created false, can not open or create data file: " + data_file_uri);
        }
    }
    
    void CleanLogFile()
    {
        string log_file_uri = CalLogFilePath(db_name_store, table_name_store);
        if (log_file_stream)
        {
            log_file_stream.close();
        }
        log_file_stream.open(log_file_uri, std::ios::trunc);
        log_file_stream.close();

        OpenLogFile();
    }

public:    // follows are functions about data

    // return true if there has next log, return false if this is the last log
    bool ReadNextLog(LogUnit& log_unit)
    {   
        std::unique_lock<mutex> lock(read_write_mutex);

        // check has next log record
        if (log_file_stream.eof())
        {
            return false;
        }
        
        // read log header
        default_length_size header_length = log_unit.header.GetHeaderLength();
        char* log_header_buffer = new char[header_length];
        log_file_stream.read(log_header_buffer, header_length);
        log_unit.header.Deserialize(log_header_buffer);
        delete[] log_header_buffer;

        // read log body
        if (log_unit.header.log_type == DELETE_LOG)
        {
            log_unit.log_body = nullptr;
        }
        else
        {
            log_unit.log_body = new char[log_unit.header.log_data_length];
            log_file_stream.read(log_unit.log_body, log_unit.header.log_data_length);
        }

        return true;
    }

    // write log unit at the file tail
    void WriteLog(LogUnit& log_unit)
    {
        std::unique_lock<mutex> lock(read_write_mutex);
        
        default_address_type cache_read_address = log_file_stream.tellg();

        char* unit_buffer;
        log_unit.Serialize(unit_buffer);

        log_file_stream.write(unit_buffer, log_unit.GetLength());

        delete[] unit_buffer;
        log_file_stream.seekg(cache_read_address, std::ios::beg);
    }

    // clean all logs, write logs back to table block, the clean strategy will designed in upper component
    void WriteBackToTable()
    {
        if (!data_file_stream)
        {
            OpenTableDataFile();
        }

        log_file_stream.seekg(0, std::ios::beg);

        LogUnit log_unit;
        while(ReadNextLog(log_unit))
        {
            // update one log record to table data file.
            DataBlock block;
            bfmm->ReadFromFile(data_file_stream, log_unit.header.block_address, block.data);
            block.DeserializeFromBuffer(block.data);
            
            switch (log_unit.header.log_type)
            {
            case UPDATE_LOG:
                // update the record data
                memcpy(block.data + log_unit.header.record_address, log_unit.log_body, block.field_length);
                break;
            case DELETE_LOG:
                // delete the record, earse from the block and re-organize block data to avoid fragment
                block.DeleteData(log_unit.header.record_address);
                break;
            default:
                throw std::runtime_error("can not found the log operation type");
            }

            bfmm->WriteBackBlock(data_file_stream, log_unit.header.block_address, block.data);
        }

        // clean the log file
        CleanLogFile();
    }


};

}

#endif // VDBMS_META_LOG_H_