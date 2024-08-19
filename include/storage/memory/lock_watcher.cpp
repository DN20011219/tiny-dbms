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

    slot_tool = new SlotTool();
    cal_url_util = new CalFileUrlUtil();
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

    delete slot_tool;
    delete cal_url_util;
    delete buffer_pool;
    delete bfmm;
    delete cal_url_util;
}

void LockWatcher::LoadBlockForRead(std::string db_name, std::string table_name, default_address_type offset, DataBlock& block)
{
    SlotSign sign = slot_tool->GetSign(db_name, table_name, offset);

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
        slots_map_mutex.unlock();
        buffer_pool->WaitForSpace();
        slots_map_mutex.lock();
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
    SlotSign sign = slot_tool->GetSign(db_name, table_name, offset);
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
        slots_map_mutex.unlock();
        buffer_pool->WaitForSpace();
        slots_map_mutex.lock();
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

void LockWatcher::LoadBlockForRead(std::string db_name, std::string table_name, default_address_type offset, TableBlock& block)
{
    SlotSign sign = slot_tool->GetSign(db_name, table_name + ".header", offset);
    BlockSlot* slot;
    
    // firstly check in map
    slots_map_mutex.lock();

    if (slots_map.find(sign) != slots_map.end())
    {   
        slot = slots_map[sign];
        slot->read_or_write_mutex.lock_shared();

        slot->in_use = true;
        slot->user_amount++;
        block.data = slot->data;
        
        block.DeserializeFromBuffer(block.data);

        return;
    }

    // get one free slot
    while (!buffer_pool->GetFreeSlot(slot))
    {
        slots_map_mutex.unlock();
        buffer_pool->WaitForSpace();
        slots_map_mutex.lock();
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
    bfmm->ReadOneTableBlock(cal_url_util->GetTableHeaderFile(db_name), offset, block);

    slots_map_mutex.unlock();
}   

void LockWatcher::LoadBlockForWrite(std::string db_name, std::string table_name, default_address_type offset, TableBlock& block)
{
    SlotSign sign = slot_tool->GetSign(db_name, table_name + ".header", offset);
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
        slots_map_mutex.unlock();
        buffer_pool->WaitForSpace();
        slots_map_mutex.lock();
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
    bfmm->ReadOneTableBlock(cal_url_util->GetTableHeaderFile(db_name), offset, block);

    slots_map_mutex.unlock();
}

bool LockWatcher::UpgradeLock(std::string db_name, std::string table_name, default_address_type offset)
{
    return true;
}

/**
 * @brief: 
 */
void LockWatcher::ReleaseReadingBlock(std::string db_name, std::string table_name, default_address_type offset, DataBlock& block)
{   
    SlotSign sign = slot_tool->GetSign(db_name, table_name, offset);

    slots_map_mutex.lock();

    if (slots_map.find(sign) != slots_map.end())
    {
        BlockSlot* slot = slots_map[sign];
        slot->user_amount--;
        if (slot->user_amount == 0)
        {
            slot->in_use = false;
        } 

        // remove from map
        slots_map.erase(sign);
        // release lock
        slot->read_or_write_mutex.unlock_shared();
    }

    slots_map_mutex.unlock();
}

void LockWatcher::ReleaseReadingBlock(std::string db_name, std::string table_name, default_address_type offset, TableBlock& block)
{   
    SlotSign sign = slot_tool->GetSign(db_name, table_name + ".header", offset);

    slots_map_mutex.lock();

    if (slots_map.find(sign) != slots_map.end())
    {
        BlockSlot* slot = slots_map[sign];
        slot->user_amount--;
        if (slot->user_amount == 0)
        {
            slot->in_use = false;
        } 

        // remove from map
        slots_map.erase(sign);
        // release lock
        slot->read_or_write_mutex.unlock_shared();
    }

    slots_map_mutex.unlock();
}

void LockWatcher::ReleaseWritingBlock(std::string db_name, std::string table_name, default_address_type offset, DataBlock& block)
{
    block.Serialize();

    // flush to disk
    SlotSign sign = slot_tool->GetSign(db_name, table_name, offset);

    slots_map_mutex.lock();

    if (slots_map.find(sign) != slots_map.end())
    {
        BlockSlot* slot = slots_map[sign];
        // flush to disk
        bfmm->WriteBackDataBlock(cal_url_util->GetTableDataFile(db_name, table_name), offset, slots_map[sign]->data);
        
        // use buffer pool to realse slot
        buffer_pool->ReleaseSlot(slot);

        // remove from map
        slots_map.erase(sign);
        // release lock
        slot->read_or_write_mutex.unlock();
    }

    slots_map_mutex.unlock();
}

void LockWatcher::ReleaseWritingBlock(std::string db_name, std::string table_name, default_address_type offset, TableBlock& block)
{
    block.SerializeHeader();

    // flush to disk
    SlotSign sign = slot_tool->GetSign(db_name, table_name + ".header", offset);

    slots_map_mutex.lock();

    if (slots_map.find(sign) != slots_map.end())
    {
        BlockSlot* slot = slots_map[sign];
        // flush to disk
        bfmm->WriteBackTableBlock(cal_url_util->GetTableHeaderFile(db_name), offset, block);
        
        // use buffer pool to realse slot
        buffer_pool->ReleaseSlot(slot);

        // remove from map
        slots_map.erase(sign);
        // release lock
        slot->read_or_write_mutex.unlock();
    }

    slots_map_mutex.unlock();
}

default_address_type LockWatcher::CreateNewBlock(std::string db_name, std::string table_name, DataBlock& block)
{
    slots_map_mutex.lock();

    BlockSlot* slot;
    
    default_address_type new_block_offset = bfmm->GetNewBlockAddress(cal_url_util->GetTableDataFile(db_name, table_name));

    SlotSign sign = slot_tool->GetSign(db_name, table_name, new_block_offset);

    // get one free slot
    while (!buffer_pool->GetFreeSlot(slot))
    {
        slots_map_mutex.unlock();
        buffer_pool->WaitForSpace();
        slots_map_mutex.lock();
    }

    // update slot information
    slot->read_or_write_mutex.lock();
    slot->in_use = true;
    slot->user_amount++;

    // update map
    slots_map[sign] = slot;
    // set pointer
    block.data = slot->data;

    // bfmm->WriteBackDataBlock(cal_url_util->GetTableDataFile(db_name, table_name), new_block_offset, block);

    slots_map_mutex.unlock();

    return new_block_offset;
}

default_address_type LockWatcher::CreateNewBlock(std::string db_name, std::string table_name, TableBlock& block)
{
    slots_map_mutex.lock();

    BlockSlot* slot;

    default_address_type new_block_offset = bfmm->GetNewBlockAddress(cal_url_util->GetTableHeaderFile(db_name));

    SlotSign sign = slot_tool->GetSign(db_name, table_name+ ".header", new_block_offset);

    // get one free slot
    while (!buffer_pool->GetFreeSlot(slot))
    {
        slots_map_mutex.unlock();
        buffer_pool->WaitForSpace();
        slots_map_mutex.lock();
    }

    // update slot information
    slot->read_or_write_mutex.lock();
    slot->in_use = true;
    slot->user_amount++;

    // update map
    slots_map[sign] = slot;
    // set pointer and init block
    block.table_amount = 0;
    block.data = slot->data;

    // bfmm->WriteBackTableBlock(cal_url_util->GetTableHeaderFile(db_name), new_block_offset, block);

    slots_map_mutex.unlock();

    return new_block_offset;
}

}