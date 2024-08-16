// Copyright (c) 2024 by dingning
//
// file  : easy_replacer.h
// since : 2024-08-15
// desc  : TODO.

#ifndef VDBMS_STORAGE_MEMORY_EASY_REPLACER_H_
#define VDBMS_STORAGE_MEMORY_EASY_REPLACER_H_

#include <string>
#include <list>


#include "./block_slot.h"
#include "../../config.h"

namespace tiny_v_dbms {

class Replacer
{

private:
    std::list<BlockSlot*> slots_read_list;

public:
    void ReadOne(BlockSlot* slot);

    std::list<BlockSlot*> GetFreeSlots();
};

}

#endif // VDBMS_STORAGE_MEMORY_EASY_REPLACER_H_