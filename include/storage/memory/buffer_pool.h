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

    std::mutex no_space_failure_times_mutex;
    int no_space_failure_times;
    
    const static int MAX_FAIL_TIMES = 20;

public:
    BufferPool(std::vector<BlockSlot*>* slots);

    // try get one free slot
    bool GetFreeSlot(BlockSlot*& slot);

    // release one slot
    void ReleaseSlot(BlockSlot*& slot);

    // try free slots right now, can only free those slots which has areadly finished use. 
    // this function will notify all threads waiting for space, and they will compete for space again.
    std::list<BlockSlot*> FreeSpace();

    // make now thread wait for space.
    void WaitForSpace();

    // wake up all thread waiting for space, make them try allocate slot again.
    void WakeUpWaitingThread();
};

}

#endif // VDBMS_STORAGE_MEMORY_BUFFER_POOL_H_