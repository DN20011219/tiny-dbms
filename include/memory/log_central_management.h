// Copyright (c) 2024 by dingning
//
// file  : log_central_management.h
// since : 2024-07-29
// desc  : Because the log file is not organized by block, and the LogManager 
// which is used to read or write log file will increase having no limit if 
// not organized by one central. So this management will manage the LogManager in 
// this dbms

#ifndef VDBMS_MEMORY_LOG_CENTRAL_MANAGEMENT_H_
#define VDBMS_MEMORY_LOG_CENTRAL_MANAGEMENT_H_

#include <map>
#include <vector>
#include <string>

#include "../meta/log.h"
#include "log_manager_slot.h"
#include "replacer/basic_log_manager_replacer.h"
#include "../config.h"

using std::map;
using std::string;
using std::vector;

namespace tiny_v_dbms {

class LogCentralManagement
{
private:
    
    default_amount_type logm_amount;
    default_amount_type used_amount;

    map<string, LogManagerSlot*> logm_map;  // table_name & LogManager
    vector<LogManagerSlot*> log_managers;

    BasicLogManagerReplacer* replacer;

    static LogCentralManagement* instance;
    static mutex singlion_mutex;

    LogCentralManagement()
    {
        logm_amount = LOG_MANAGER_INSRANCE_AMOUNT;
        used_amount = 0;

        replacer = new BasicLogManagerReplacer(LOG_MANAGER_INSRANCE_AMOUNT, logm_map, log_managers);
    }

    ~LogCentralManagement()
    {
        for (default_amount_type i = 0; i < log_managers.size(); i++)
        {
            delete log_managers[i];
        }
    }

public:

    static LogCentralManagement* GetInstance()
    {
        if (instance == nullptr)
        {
            std::unique_lock<mutex> lock(singlion_mutex);
            if (instance == nullptr)
            {
                instance = new (std::nothrow) LogCentralManagement();
            }
        }
        return instance;
    }

    void deleteInstance()
    {
        std::unique_lock<std::mutex> lock(singlion_mutex);
        if (instance)
        {
            delete instance;
            instance = nullptr;
        }
    }

    LogManager* GetLogManager(string db_name, string table_name) 
    {
        LogManager* log_m = replacer->GetLogManager(db_name, table_name);
        if (log_m == nullptr)
        {
            throw std::runtime_error("Can not get free LogManager!");
        }
        return log_m;
    }

    bool ReleaseLogManager(string db_name, string table_name)
    {
        return replacer->FreeLogManager(db_name, table_name);
    }

};

}

#endif // VDBMS_MEMORY_LOG_CENTRAL_MANAGEMENT_H_