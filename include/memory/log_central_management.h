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
#include "../config.h"

using std::map;
using std::string;
using std::vector;

namespace tiny_v_dbms {

class LogCentralManagement
{
private:
    vector<LogManagerSlot> log_managers;
    default_amount_type logm_amount;
    default_amount_type used_amount;

    map<string, LogManagerSlot> logm_map;  // table_name & LogManager

public:

    LogCentralManagement()
    {
        logm_amount = LOG_MANAGER_INSRANCE_AMOUNT;
        used_amount = 0;
    }

    ~LogCentralManagement()
    {
        
    }

    bool GetLogManager()
    {
        
    }

    bool ReleaseLogManager()
    {

    }
};

}

#endif // VDBMS_MEMORY_LOG_CENTRAL_MANAGEMENT_H_