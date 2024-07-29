// Copyright (c) 2024 by dingning
//
// file  : basic_log_manager_replacer.h
// since : 2024-07-29
// desc  : TODO.

#ifndef VDBMS_MEMORY_BLOCK_BASIC_LOG_MANAGER_REPLACER_H_
#define VDBMS_MEMORY_BLOCK_BASIC_LOG_MANAGER_REPLACER_H_

#include <map>
#include <string>
#include <vector>

#include "../log_manager_slot.h"

using std::map;
using std::string;
using std::vector;

namespace tiny_v_dbms {

class BasicLogManagerReplacer
{

private:

    map<string, LogManagerSlot*> logm_map_store; // this is a map, which stores the (db_name + table_name, LogManagerSlot) as one item
    vector<LogManagerSlot*> log_managers_store;

    default_amount_type logm_amount;
    default_amount_type max_logm_amount;

    vector<LogManagerSlot*> using_logm;
    vector<LogManagerSlot*> free_logm;

public:

    BasicLogManagerReplacer(default_amount_type max_amount, map<string, LogManagerSlot*>& logm_map, vector<LogManagerSlot*>& log_managers)
    {
        logm_amount = 0;
        max_logm_amount = max_amount;

        logm_map_store = logm_map;
        log_managers_store = log_managers;
    }
    
    LogManager* GetLogManager(string db_name, string table_name)
    {   
        string name = db_name + "&" + table_name;
        // search in exist map
        if (logm_map_store[name] != nullptr)
        {   
            // if it areadly exist and unused
            if (logm_map_store[name]->is_free)
            {
                return logm_map_store[name]->log_manager;
            }
            throw std::runtime_error("this log manger has areadly been used");
        }

        // try create one new slot
        if (free_logm.empty())
        {
            // create one new slot, and fill it with the new LogManager, add it to map and used vector
            if (logm_amount < max_logm_amount)
            {
                LogManagerSlot* new_slot = new LogManagerSlot();
                
                logm_map_store.insert(std::make_pair(name, new_slot));
                using_logm.push_back(new_slot);

                // create new LogManager
                std::unique_lock<mutex> lock(new_slot->access_control); // escape create many times
                new_slot->log_manager = new LogManager(db_name, table_name);

                logm_amount++;

                return new_slot->log_manager;
            }
        }
        else
        {
            // get from free list
            LogManagerSlot* slot = free_logm[free_logm.size()];
            free_logm.pop_back();
            
            // init and mark use state
            std::unique_lock<mutex> lock(slot->access_control); // escape create many times
            slot->log_manager = new LogManager(db_name, table_name);
            slot->is_free = false;

            // push to using list
            using_logm.push_back(slot); // push to using_list

            // add to map
            logm_map_store.insert(std::make_pair(name, slot));

            return slot->log_manager;
        }

        return nullptr;
    }

    bool FreeLogManager(string db_name, string table_name)
    {
        string name = db_name + "&" + table_name;

        // if it exist in mangement
        if (logm_map_store.find(name) != logm_map_store.end())
        {   
            LogManagerSlot* slot = logm_map_store[name];

            std::unique_lock<mutex> lock(slot->access_control);

            // free LogManager
            free(slot->log_manager);

            // set state
            slot->is_free = true;

            // clean control struct
            logm_map_store.erase(name); // clean in map

            free_logm.push_back(slot);  // add to free_list
            
            auto iter = using_logm.begin(); // clean in using_list
            while (iter != using_logm.end())
            {
                if (*iter == slot)
                {
                    using_logm.erase(iter);
                    break;
                }
                iter++;
            }

            return true;
        }

        return false;
    }

};

}

#endif // VDBMS_MEMORY_BLOCK_BASIC_LOG_MANAGER_REPLACER_H_