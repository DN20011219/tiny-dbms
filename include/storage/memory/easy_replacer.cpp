// Copyright (c) 2024 by dingning
//
// file  : easy_replacer.cpp
// since : 2024-08-15
// desc  : TODO.

#include "./easy_replacer.h"


namespace tiny_v_dbms {


void Replacer::ReadOne(BlockSlot* slot)
{
    for (auto it = slots_read_list.begin(); it != slots_read_list.end(); ++it) {
        if (slot == *it) {
            slots_read_list.erase(it);
            break;
        }
    }

    // insert to front
    slots_read_list.push_front(slot);
}

std::list<BlockSlot*> Replacer::GetFreeSlots()
{
    std::list<BlockSlot*> free_list;
    for (auto it = slots_read_list.begin(); it != slots_read_list.end(); ++it) {
        if ((*it)->in_use == false) {
            slots_read_list.erase(it);
            free_list.push_back(*it);
        }
    }
    return free_list;
}

}
