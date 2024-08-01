// Copyright (c) 2024 by dingning
//
// file  : log_manager_slot.h
// since : 2024-07-29
// desc  : the slot that store LogManager and use state.

#ifndef VDBMS_MEMORY_LOG_MANAGEMER_SLOT_H_
#define VDBMS_MEMORY_LOG_MANAGEMER_SLOT_H_

#include "../meta/log.h"

using std::mutex;

namespace tiny_v_dbms {

struct LogManagerSlot 
{

public:
    bool is_free;                       // if is free, then can use it.
    mutex access_control;               // lock one block, so other thread can not change or readit.

    LogManager* log_manager;



    LogManagerSlot()
    {
        is_free = true;
    }

    ~LogManagerSlot()
    {
        if (log_manager)
        {
            delete log_manager;
        }
    }

};

}

#endif // VDBMS_MEMORY_LOG_MANAGEMER_SLOT_H_