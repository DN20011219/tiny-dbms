// #include <iostream>

// #include "../../include/memory/memory_management.h"


// namespace tiny_v_dbms {
        
//     BufferPool *BufferPool::m_SingleInstance = NULL;
//     std::mutex BufferPool::m_Mutex;

//     /**
//      * @brief Constructor for BufferPool class
//      * 
//      * Initializes the memory management system with default settings.
//      * 
//      * @example
//      * BufferPool mm; // Create an instance of BufferPool
//     */
//     BufferPool::BufferPool()
//     {   
//         std::cout << "SingleInstance BufferPool begin create" << std::endl;

//         default_length_size block_size = BLOCK_SIZE;

//         // init data blocks slots
//         default_length_size total_space = (default_length_size) MEMORY_SIZE;
//         max_data_block_amount = (total_space / 4 * 3) / MemoryBlockSlot::GetSize(); // use max 3/4 of total space
//         data_memory_blocks = new MemoryBlockSlot[max_data_block_amount];
//         for(int i = 0; i < max_data_block_amount; i++) {
//             data_memory_blocks[i].data = new char[block_size];
//             free_data_blocks.push_back(& data_memory_blocks[i]);
//         }

//         // init table blocks slots
//         max_table_block_amount = (total_space / 8 * 1) / MemoryBlockSlot::GetSize(); // use max 1/8 of total space
//         table_memory_blocks = new MemoryBlockSlot[max_table_block_amount];
//         for(int i = 0; i < max_table_block_amount; i++) {
//             table_memory_blocks[i].data = new char[block_size];
//             free_table_blocks.push_back(& table_memory_blocks[i]);
//         }

//         AnalysizeSpaceUsed();

//         // so can use different replace strategy in different type memory
//         // BlockReplacer replacer_one(data_memory_blocks, max_data_block_amount, free_data_blocks, using_data_blocks);
//         // data_replacer = new BlockBasicReplacer(data_memory_blocks, max_data_block_amount, free_data_blocks, using_data_blocks);

//         // BlockReplacer replacer_two(table_memory_blocks, max_table_block_amount, free_table_blocks, using_table_blocks);
//         // table_replacer = new BlockBasicReplacer(table_memory_blocks, max_table_block_amount, free_table_blocks, using_table_blocks);

//         std::cout << "SingleInstance BufferPool end create" << std::endl;
//     }


//     BufferPool *&BufferPool::GetInstance()
//     {
//         if (m_SingleInstance == NULL) 
//         {
//             std::unique_lock<std::mutex> lock(m_Mutex); // 加锁
//             if (m_SingleInstance == NULL)
//             {
//                 m_SingleInstance = new (std::nothrow) BufferPool;
//             }
//         }

//         return m_SingleInstance;
//     }

//     void BufferPool::deleteInstance()
//     {
//         std::unique_lock<std::mutex> lock(m_Mutex);
//         if (m_SingleInstance)
//         {
//             delete m_SingleInstance;
//             m_SingleInstance = NULL;
//         }
//     }
// }