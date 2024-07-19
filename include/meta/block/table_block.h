// Copyright (c) 2024 by dingning
//
// file  : table_block.h
// since : 2024-07-18
// desc  : Basic unit to read from disk or write to disk. This
// type block mainly store the information ablout table, so called table block.
// Information about table is stored one by one, but the length of different 
// tables are different. So need to store the nums of tables.
/*

| address |  data description |

0 ---------block begin 4kb----------------------
0 | table amount (2) |
4 | free space (4096 - 4*3 - 4*2 - (4096 - 4079 - 1) - (4079 - 4038 - 1)) |
8 | next block address (null) |
12 | table1 start address (4079) |
16 | table2 start address (4038) |

20 | free space [8 - 4038) |

4038 | table2 meta data (contains 40 byte data)|

4079 | table1 meta data (contains 16 byte data)|

4096 ------block end----------------------------

*/

#ifndef VDBMS_META_BLOCK_TABLE_BLOCK_H_
#define VDBMS_META_BLOCK_TABLE_BLOCK_H_

#include "../table/column_table.h"

namespace tiny_v_dbms {

struct TableBlock 
{

public:
    // static space
    default_amount_type table_amount;           // amount of tables store in here
    default_length_size free_space;             // free space of this block, unit is byte
    default_address_type next_block_pointer;    // store a pointer to next block, if has next block   

    // dynamic space
    default_address_type* tables_begin_address; // store the begin address of each table

    void CalAndUpdateFreeSpace() {

        free_space = (int)BLOCK_SIZE - sizeof(default_amount_type) - sizeof(default_length_size) - sizeof(default_address_type);

        if (table_amount != 0) {
            free_space -= BLOCK_SIZE - tables_begin_address[table_amount - 1];
        }
    }

};

}

#endif // VDBMS_META_BLOCK_TABLE_BLOCK_H_