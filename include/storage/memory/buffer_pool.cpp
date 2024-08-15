// Copyright (c) 2024 by dingning
//
// file  : buffer_pool.cpp
// since : 2024-08-15
// desc  : TODO.

#include "./buffer_pool.h"

namespace tiny_v_dbms {


BufferPool::BufferPool(std::vector<BlockSlot*>* slots) : slots(slots)
{
    replacer = new Replacer();
}

bool BufferPool::GetFreeSlot(BlockSlot*& slot)
{
    std::unique_lock<std::mutex> slots_lock(slots_mutex);

    for (auto& item : *slots)
    {
        if (!item->in_use)
        {
            slot = item;
            replacer->ReadOne(slot);
            return true;
        }
    }

    return false;
}

bool BufferPool::ReleaseSlot(BlockSlot*& slot)
{
    slot->in_use = false;
    slot->user_amount = 0;

    WakeUpWaitingThread();
}

std::list<BlockSlot*> BufferPool::FreeSpace(BlockSlot*& olest)
{
    std::list<BlockSlot*> free_list = replacer->GetFreeSlots();
    // for (auto item : free_list)
    // {
    //     ReleaseSlot(item);
    // }

    // if (free_list.size() == 1)
    // {
    //     no_space_cv.notify_one();
    // }
    // else
    // {
    //     no_space_cv.notify_all();
    // }
    
    return free_list;
}

void BufferPool::WaitForSpace()
{
    std::unique_lock<std::mutex> lock(no_space_mutex);
    no_space_cv.wait(lock);
}

void BufferPool::WakeUpWaitingThread()
{
    no_space_cv.notify_one();
}

}