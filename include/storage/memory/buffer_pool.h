// Copyright (c) 2024 by dingning
//
// file  : buffer_pool.h
// since : 2024-08-15
// desc  : TODO.

#ifndef VDBMS_STORAGE_MEMORY_BUFFER_POOL_H_
#define VDBMS_STORAGE_MEMORY_BUFFER_POOL_H_

#include <vector>
#include <mutex>
#include <condition_variable>

#include "./block_slot.h"
#include "./easy_replacer.h"

namespace tiny_v_dbms {

class BufferPool
{

private:
    std::vector<BlockSlot*>* slots;
    Replacer* replacer;
    std::mutex slots_mutex;
    std::mutex no_space_mutex;
    std::condition_variable no_space_cv;

public:
    BufferPool(std::vector<BlockSlot*>* slots);

    bool GetFreeSlot(BlockSlot*& slot);

    bool ReleaseSlot(BlockSlot*& slot);

    std::list<BlockSlot*> FreeSpace(BlockSlot*& olest);

    void WaitForSpace();
    void WakeUpWaitingThread();
};

}

#endif // VDBMS_STORAGE_MEMORY_BUFFER_POOL_H_