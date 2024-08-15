// Copyright (c) 2024 by dingning
//
// file  : lock_watcher.cpp
// since : 2024-08-15
// desc  : TODO.

#include <string>
#include <vector>

#include "./lock_watcher.h"

namespace tiny_v_dbms {

LockWatcher::LockWatcher(default_amount_type slots_amount)
{
    while (slots_amount > 0)
    {
        slots.push_back(new BlockSlot());
        slots_amount--;
    }

    buffer_pool = new BufferPool(&slots);
    bfmm = new BlockFileManagement();
    cal_url_util = new CalFileUrlUtil();
}

LockWatcher::~LockWatcher()
{
    for (auto& item: slots)
    {
        item->read_or_write_mutex.lock();
        delete item;
    }

    delete buffer_pool;
    delete bfmm;
    delete cal_url_util;
}

void LockWatcher::LoadBlockForRead(std::string db_name, std::string table_name, default_address_type offset, DataBlock& block)
{
    SlotSign sign = SlotTool::GetSign(db_name, table_name, offset);

    // firstly check in map
    slots_map_mutex.lock();

    if (slots_map.find(sign) != slots_map.end())
    {   
        slots_map[sign]->read_or_write_mutex.lock_shared();

        slots_map[sign]->in_use = true;
        slots_map[sign]->user_amount++;
        block.data = slots_map[sign]->data;
        block.DeserializeFromBuffer(block.data);

        return;
    }

    BlockSlot* slot;
    // get one free slot
    while (!buffer_pool->GetFreeSlot(slot))
    {
        buffer_pool->WaitForSpace();
    }

    // update slot information
    slot->read_or_write_mutex.lock_shared();
    slot->in_use = true;
    slot->user_amount++;

    // update map
    slots_map[sign] = slot;
    // set pointer
    block.data = slot->data;
    // load data
    bfmm->ReadOneDataBlock(cal_url_util->GetTableDataFile(db_name, table_name), offset, block);

    slots_map_mutex.unlock();
}   


void LockWatcher::LoadBlockForWrite(std::string db_name, std::string table_name, default_address_type offset, DataBlock& block)
{
    SlotSign sign = SlotTool::GetSign(db_name, table_name, offset);
    BlockSlot* slot;

    // firstly check in map
    slots_map_mutex.lock();

    if (slots_map.find(sign) != slots_map.end())
    {   
        slots_map[sign]->read_or_write_mutex.lock();

        slot = slots_map[sign];
        slot->in_use = true;
        slot->user_amount = 1;
        block.data = slot->data;
        block.DeserializeFromBuffer(block.data);

        return;
    }

    // get one free slot
    while (!buffer_pool->GetFreeSlot(slot))
    {
        buffer_pool->WaitForSpace();
    }

    // update slot information
    slot->read_or_write_mutex.lock();
    slot->in_use = true;
    slot->user_amount = 1;

    // update map
    slots_map[sign] = slot;
    // set pointer
    block.data = slot->data;
    // load data
    bfmm->ReadOneDataBlock(cal_url_util->GetTableDataFile(db_name, table_name), offset, block);

    slots_map_mutex.unlock();
}

bool LockWatcher::UpgradeLock(std::string db_name, std::string table_name, default_address_type offset)
{
    
}

void LockWatcher::ReleaseReadingBlock(std::string db_name, std::string table_name, default_address_type offset)
{   
    SlotSign sign = SlotTool::GetSign(db_name, table_name, offset);

    slots_map_mutex.lock();

    if (slots_map.find(sign) != slots_map.end())
    {
        slots_map[sign]->user_amount--;
        if (slots_map[sign]->user_amount == 0)
        {
            slots_map[sign]->in_use = false;
        } 

        // release lock
        slots_map[sign]->read_or_write_mutex.unlock_shared();
    }

    slots_map_mutex.unlock();
}

/**
 * 
 * must deserialize data before release
 */
void LockWatcher::ReleaseWritingBlock(std::string db_name, std::string table_name, default_address_type offset)
{
    // flush to disk
    SlotSign sign = SlotTool::GetSign(db_name, table_name, offset);

    slots_map_mutex.lock();

    if (slots_map.find(sign) != slots_map.end())
    {
        slots_map[sign]->user_amount = 0;
        slots_map[sign]->in_use = false;

        // flush to disk
        bfmm->WriteBackDataBlock(cal_url_util->GetTableDataFile(db_name, table_name), offset, slots_map[sign]->data);
        // release lock
        slots_map[sign]->read_or_write_mutex.unlock_shared();
    }

    slots_map_mutex.unlock();
}

void LockWatcher::CreateNewBlock(std::string db_name, std::string table_name, default_address_type offset, DataBlock& block)
{
    
}

}