// Copyright (c) 2024 by dingning
//
// file  : block_slot.h
// since : 2024-08-15
// desc  : TODO.

#ifndef VDBMS_STORAGE_MEMORY_BLOCK_SLOT_H_
#define VDBMS_STORAGE_MEMORY_BLOCK_SLOT_H_

#include <string>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>

#include "../../config.h"

namespace tiny_v_dbms {

struct SlotSign
{
    std::string db_name;
    std::string table_name;
    default_address_type block_offset;

    bool operator<(const SlotSign& other) const {
        if (db_name < other.db_name) return true;
        if (db_name > other.db_name) return false;
        if (table_name < other.table_name) return true;
        if (table_name > other.table_name) return false;
        return block_offset < other.block_offset;
    }
};

struct BlockSlot
{
    // SlotSign block_sign; // db_name&&table_name&&block_offset
    char* data;

    // information about replace
    bool in_use;
    bool is_dirty;
    int user_amount;

    // tools for cocurrent contro
    std::shared_mutex read_or_write_mutex;

    // std::mutex update_struct_mutex; // used for thread waiting or awaken
    // std::condition_variable update_struct_cv;

    BlockSlot()
    {
        data = new char[BLOCK_SIZE];
    }

    ~BlockSlot()
    {
        delete[] data;
        read_or_write_mutex.unlock();
    }

    void Clear()
    {
        memcpy(data, 0, BLOCK_SIZE);
    }
};

class SlotTool
{
public:
    SlotSign GetSign(std::string db_name, std::string table_name, default_address_type offset);
};


}

#endif // VDBMS_STORAGE_MEMORY_BLOCK_SLOT_H_