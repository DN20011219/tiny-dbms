#include <iostream>

#include "../../include/memory/memory_management.h"


namespace tiny_v_dbms {
        
    MemoryManagement *MemoryManagement::m_SingleInstance = NULL;
    std::mutex MemoryManagement::m_Mutex;

    /**
     * @brief Constructor for MemoryManagement class
     * 
     * Initializes the memory management system with default settings.
     * 
     * @example
     * MemoryManagement mm; // Create an instance of MemoryManagement
    */
    MemoryManagement::MemoryManagement()
    {   
        std::cout << "SingleInstance MemoryManagement begin create" << std::endl;

        default_length_size block_size = BLOCK_SIZE;

        // init data blocks slots
        default_length_size total_space = (default_length_size) MEMORY_SIZE;
        max_data_block_amount = (total_space / 4 * 3) / MemoryBlock::GetSize(); // use max 3/4 of total space
        data_memory_blocks = new MemoryBlock[max_data_block_amount];
        for(int i = 0; i < max_data_block_amount; i++) {
            data_memory_blocks[i].data = new char[block_size];
            free_data_blocks.push_back(& data_memory_blocks[i]);
        }

        // init table blocks slots
        max_table_block_amount = (total_space / 8 * 1) / MemoryBlock::GetSize(); // use max 1/8 of total space
        table_memory_blocks = new MemoryBlock[max_table_block_amount];
        for(int i = 0; i < max_table_block_amount; i++) {
            table_memory_blocks[i].data = new char[block_size];
            free_table_blocks.push_back(& table_memory_blocks[i]);
        }

        AnalysizeSpaceUsed();

        // so can use different replace strategy in different type memory
        // BlockReplacer replacer_one(data_memory_blocks, max_data_block_amount, free_data_blocks, using_data_blocks);
        data_replacer = new BlockReplacer(data_memory_blocks, max_data_block_amount, free_data_blocks, using_data_blocks);

        // BlockReplacer replacer_two(table_memory_blocks, max_table_block_amount, free_table_blocks, using_table_blocks);
        table_replacer = new BlockReplacer(table_memory_blocks, max_table_block_amount, free_table_blocks, using_table_blocks);

        std::cout << "SingleInstance MemoryManagement end create" << std::endl;
    }


    MemoryManagement *&MemoryManagement::GetInstance()
    {
        if (m_SingleInstance == NULL) 
        {
            std::unique_lock<std::mutex> lock(m_Mutex); // 加锁
            if (m_SingleInstance == NULL)
            {
                m_SingleInstance = new (std::nothrow) MemoryManagement;
            }
        }

        return m_SingleInstance;
    }

    void MemoryManagement::deleteInstance()
    {
        std::unique_lock<std::mutex> lock(m_Mutex);
        if (m_SingleInstance)
        {
            delete m_SingleInstance;
            m_SingleInstance = NULL;
        }
    }
}