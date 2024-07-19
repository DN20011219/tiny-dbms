// Copyright (c) 2024 by dingning
//
// file  : block_replace.h
// since : 2024-07-TODO
// desc  : TODO.

#ifndef VDBMS_MEMORY_BLOCK_REPLACER_H_
#define VDBMS_MEMORY_BLOCK_REPLACER_H_

#include <map>

#include "memory_ management.h"


namespace tiny_v_dbms {

class BlockReplacer
{
private:
    MemoryBlock* blocks;
    default_amount_type max_block_amount;

    list<MemoryBlock*> free_blocks;
    list<MemoryBlock*> using_blocks;

    std::map<char*, MemoryBlock*> data_block_map;

public:
    BlockReplacer(MemoryBlock* blocks, default_amount_type max_block_amount, list<MemoryBlock*>& free_blocks, list<MemoryBlock*>& using_blocks):
        blocks(blocks), max_block_amount(max_block_amount), free_blocks(free_blocks), using_blocks(using_blocks)
    {
        // cache all blocks add pointer and block map, so can find block using data pointer, could be used in release block
        for(int i = 0; i < max_block_amount; i++)
        {
            char* data= blocks[max_block_amount].data;
            std::pair<char*, MemoryBlock*> pair(data, &blocks[max_block_amount]);
            data_block_map.insert(pair);
        }
    }

    // if has one free block, return true and change data to the data pointer of free block, at the same time change the information on block.
    // if not, return false.
    bool GetFreeBlock(char*& data)
    {
        if(free_blocks.empty())
        {
            return false;
        }
        MemoryBlock* block = free_blocks.front();
        free_blocks.pop_front();
        using_blocks.push_back(block);

        block->access_control.lock();
        block->is_free = false;
        block->user_amount = 1;

        data = block->data;

        return true;
    }
    
    // release one block
    // if not controlled by replacer, do nothing, return false
    // else release it, delete it from used list and add to free list,unlock the mutex, set information on block empty, return true
    bool ReleaseBlock(char* data)
    {
        if (data_block_map.find(data) != data_block_map.end())
        {
            data_block_map[data]->is_free = true;
            data_block_map[data]->user_amount = 0;
            data_block_map[data]->access_control.unlock();

            // find in used list
            std::list<MemoryBlock*>::iterator erase_block;
            for (std::list<MemoryBlock*>::iterator it = using_blocks.begin(); it != using_blocks.end(); ++it) {
                if (data_block_map[data] == *it)
                {
                    erase_block = it;
                }
            }
            free_blocks.push_back(*erase_block);
            using_blocks.erase(erase_block);
            
            return true;
        }

        return false;
    }
};

}

#endif // VDBMS_MEMORY_BLOCK_REPLACER_H_