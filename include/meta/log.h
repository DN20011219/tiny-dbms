// Copyright (c) 2024 by dingning
//
// file  : log.h
// since : 2024-07-29
// desc  : this is the definition of log unit. Each table has it's log file, 
// and each change (updata, delete) of data will firstly write to the tail of the log file. 
// Then dbms will find some empty time to write the record in log file back to table data blocks.
// So can decrease the cost that flash total block data to disk, even only change one bit.


#ifndef VDBMS_META_LOG_UNIT_H_
#define VDBMS_META_LOG_UNIT_H_

#include <fstream>
#include <mutex>

#include "../storage/file_management.h"
#include "../config.h"

using std::fstream;
using std::mutex;


namespace tiny_v_dbms {

enum LogType { UPDATE, DELETE };

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

        if (log_type == DELETE)
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

        if (log_type == DELETE)
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

};

// designed to manage the read / write / clean / serialize / deserialize of the log data
class LogManager
{
    
private:
    fstream log_file_stream;
    mutex read_write_mutex;

public:

    LogManager(char* log_file_uri)
    {
        log_file_stream.open(log_file_uri, std::ios::in | std::ios::out);
        if (!log_file_stream)
        {
            FileManagement file_mm;
            file_mm.ReadOrCreateFile(log_file_uri).close();
            log_file_stream.open(log_file_uri, std::ios::in | std::ios::out);
        }
        if (!log_file_stream)
        {
            throw std::runtime_error("LogManager created false, can not open or create log file!");
        }
    }

    ~LogManager()
    {
        log_file_stream.close();
    }

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
        if (log_unit.header.log_type == DELETE)
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

    void WriteLog(LogUnit& log_unit)
    {
        std::unique_lock<mutex> lock(read_write_mutex);

        
    }

    // clean logs
    void WriteBackToTable()
    {

    }

    void Serialize()
    {

    }

    void Deserialize()
    {

    }


};


}

#endif // VDBMS_META_LOG_UNIT_H_