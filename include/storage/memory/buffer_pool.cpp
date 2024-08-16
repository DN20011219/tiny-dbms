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

            // reset failure times
            std::unique_lock<std::mutex> lock(no_space_failure_times_mutex);
            no_space_failure_times = 0;

            return true;
        }
    }

    // record the false times, if false frequence is too high (which means FreeSpace function is invalid), throw error to shut down the thread.
    std::unique_lock<std::mutex> lock(no_space_failure_times_mutex);
    no_space_failure_times++;

    if (no_space_failure_times >= MAX_FAIL_TIMES)
    {
        throw std::runtime_error("No enough memory space error!");
    } else {
        FreeSpace();
    }

    return false;
}

void BufferPool::ReleaseSlot(BlockSlot*& slot)
{
    slot->in_use = false;
    slot->user_amount = 0;
}

std::list<BlockSlot*> BufferPool::FreeSpace()
{
    std::list<BlockSlot*> free_list = replacer->GetFreeSlots();
    if (free_list.size() > 0)
    {
        WakeUpWaitingThread();
    }
    return free_list;
}

void BufferPool::WaitForSpace()
{
    std::unique_lock<std::mutex> lock(no_space_mutex);
    no_space_cv.wait(lock);
}

void BufferPool::WakeUpWaitingThread()
{
    no_space_cv.notify_all();
}

}