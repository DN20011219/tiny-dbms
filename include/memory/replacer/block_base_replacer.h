// Copyright (c) 2024 by dingning
//
// file  : block_base_replacer.h
// since : 2024-07-20
// desc  : basic replacer, this is a easy block replacer.

#ifndef VDBMS_MEMORY_BLOCK_REPALCER_BASIC_REPLACER_H_
#define VDBMS_MEMORY_BLOCK_REPALCER_BASIC_REPLACER_H_

#include <fstream>
#include <map>
#include <thread>
#include <vector>
#include <list>
#include <iostream>
#include <mutex>
#include <condition_variable>

#include "../memory_management.h"
#include "../memory_block_slot.h"
#include "../../storage/block_file_management.h"

namespace tiny_v_dbms {

using std::mutex;
using std::list;
using std::vector;
using std::condition_variable;

class BlockBasicReplacer
{
private:
    vector<MemoryBlockSlot*>* blocks;
    default_amount_type max_block_amount;

    mutex list_mutex;
    condition_variable free_slot_condition;
    list<MemoryBlockSlot*>* free_blocks;
    list<MemoryBlockSlot*>* using_blocks;
    
    mutex map_mutex;
    std::map<std::string, MemoryBlockSlot*> data_block_map;

    BlockFileManagement* bfmm;

public:
    ~BlockBasicReplacer()
    {
        std::cout << "BlockReplacer has been destoried" << std::endl;
        delete bfmm;
    }

    BlockBasicReplacer(vector<MemoryBlockSlot*>* blocks, default_amount_type max_block_amount, list<MemoryBlockSlot*>& free_blocks_list, list<MemoryBlockSlot*>& using_blocks_list):
        blocks(blocks), max_block_amount(max_block_amount)
    {   
        free_blocks = &free_blocks_list;
        using_blocks = &using_blocks_list;

        bfmm = new BlockFileManagement();
    }

    // try load the block of input table's block_offset for reading.
    // will firstly check it whether has been load to memory, if has been load, try share it.
    bool LoadBlockForRead(char*& data, std::string table_sign, default_address_type block_offset)
    {   
        std::string sign = table_sign + std::to_string(block_offset);

        // check whether this block has been opened
        std::unique_lock<mutex> map_lock(map_mutex);

        // if has been opened, try share it
        if (data_block_map.find(sign) != data_block_map.end())
        {
            // try get read lock
            while (!data_block_map[sign]->read_or_write_mutex.try_lock_shared())
            {
                // if failed, waiting for write thread's wake
                std::unique_lock<mutex> lock(data_block_map[sign]->unique_mutex);
                data_block_map[sign]->lock_condition.wait(lock);
            }

            // shared successfully, update user_amount.
            data_block_map[sign]->user_amount++;
        }

        // if this block has not been load, allocate one new block, and mark it as used
        MemoryBlockSlot* block;
        {
            std::unique_lock<mutex> list_lock(list_mutex);

            // try get one empty block
            if(free_blocks->empty())
            {   
                std::cout << "thread " << std::this_thread::get_id() << " need one free block, but has no block to use" << std::endl;

                // thread sleep, wait for free block function's wake up.
                free_slot_condition.wait(list_lock);
            }

            block = free_blocks->front();

            // lock mutex on shared mode
            block->read_or_write_mutex.lock_shared();
            block->block_sign = sign;
            block->is_free = false;
            block->user_amount += 1;

            // set data pointer
            data = block->data;

            // update allocation stratagem list
            free_blocks->pop_front();
            using_blocks->push_back(block);

            // release list_lock
        }

        // update map
        data_block_map[sign] = block;

        // free map_lock
        return true;
    }
    
    bool LoadBlockForWrite(char*& data, std::string table_sign, default_address_type block_offset)
    {   
        std::string sign = table_sign + std::to_string(block_offset);

        // check whether this block has been opened
        std::unique_lock<mutex> map_lock(map_mutex);

        // if has been opened, try share it
        while (data_block_map.find(sign) != data_block_map.end() && !data_block_map[sign]->read_or_write_mutex.try_lock())
        {
            // try get read lock
            while ()
            {
                // if failed, waiting for read thread's wake
                std::unique_lock<mutex> lock(data_block_map[sign]->unique_mutex);
                data_block_map[sign]->lock_condition.wait(lock);
            }

            // shared successfully, update user_amount.
            data_block_map[sign]->user_amount++;
        }

        // if this block has not been load, allocate one new block, and mark it as used
        MemoryBlockSlot* block;
        {
            std::unique_lock<mutex> list_lock(list_mutex);

            // try get one empty block
            if(free_blocks->empty())
            {   
                std::cout << "thread " << std::this_thread::get_id() << " need one free block, but has no block to use" << std::endl;

                // thread sleep, wait for free block function's wake up.
                free_slot_condition.wait(list_lock);
            }

            block = free_blocks->front();

            // lock mutex on shared mode
            block->block_sign = sign;
            block->read_or_write_mutex.lock();
            block->is_free = false;
            block->dirty_block = true;
            block->user_amount += 1;

            // set data pointer
            data = block->data;

            // update allocation stratagem list
            free_blocks->pop_front();
            using_blocks->push_back(block);

            // release list_lock
        }

        // update map
        data_block_map[sign] = block;

        // free map_lock
        return true;
    }

    // release one read block, try notify one write thread waiting for this block
    // if not controlled by replacer, do nothing, return false
    // else release it, delete it from used list and add to free list,unlock the mutex, set information on block empty, return true
    bool ReleaseReadBlock(std::string table_sign, default_address_type block_offset)
    {   
        std::string sign = table_sign + std::to_string(block_offset);

        std::unique_lock<mutex> map_lock(map_mutex);

        if (data_block_map.find(sign) != data_block_map.end())
        {   
            // update slot information
            std::unique_lock<mutex> unique_lock(data_block_map[sign]->unique_mutex);

            data_block_map[sign]->user_amount--;

            // if user_amount == 0, it has chance to wake up write thread
            if (data_block_map[sign]->user_amount == 0)
            {
                data_block_map[sign]->is_free = true;
                data_block_map[sign]->read_or_write_mutex.unlock_shared();

                // wake up write thread
                data_block_map[sign]->lock_condition.notify_one();
            }


            
            // try wake up one write thread
            if (data_block_map[sign]->user_amount == 0)
            {

            }

            return true;
        }

        return false;
    }

    // notify all
    bool ReleaseWriteBlock()
    {

    }

};

}

#endif // VDBMS_MEMORY_BLOCK_REPALCER_BASIC_REPLACER_H_