
#include "./block_slot.h"

namespace tiny_v_dbms {


SlotSign SlotTool::GetSign(std::string db_name, std::string table_name, default_address_type offset)
{
    SlotSign slot;
    slot.db_name = db_name;
    slot.table_name = table_name;
    slot.block_offset = offset;
    return slot;
}

}