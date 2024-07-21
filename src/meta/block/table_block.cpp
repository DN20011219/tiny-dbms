// // Copyright (c) 2024 by dingning
// //
// // file  : table_block.cpp
// // since : 2024-07-18
// // desc  : Basic unit to read from disk or write to disk. This
// // type block mainly store the information ablout table, so called table block.
// // Information about table is stored one by one, but the length of different 
// // tables are different. So need to store the nums of tables.


// #include "../../../include/meta/block/table_block.h"
// // #include "../../memory/memory_management.h"

// namespace tiny_v_dbms {

//     TableBlock::TableBlock()
//     {
//         table_amount = 0;
//         CalAndUpdateFreeSpace();
//         next_block_pointer = 0x0;
 
//         // init tables_begin_address
//         tables_begin_address = new default_address_type[table_amount];

//         // open one memory block
//         MemoryManagement* mm = MemoryManagement::GetInstance();
//         mm->GetFreeTableBlock(data);

//         // init memory space
//         default_length_size data_length = BLOCK_SIZE;
//         // data = new char[data_length];
//         std::cout << "init table block: " << data[4095] << std::endl;

//         SerializeHeader();
//     }

// }
