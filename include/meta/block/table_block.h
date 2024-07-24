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
#include "../../memory/memory_management.h"

namespace tiny_v_dbms {

class TableBlock 
{
public:

    // static space
    default_amount_type table_amount;           // amount of tables store in here
    default_length_size free_space;             // free space of this block, unit is byte
    default_address_type next_block_pointer;    // store a pointer to next block, if has next block   
    // dynamic space
    default_address_type* tables_begin_address; // store the begin address of each table, its the offset from block begin

    // not serialize field
    char* data;                                 // data pointer in memory, used to visit memory

    TableBlock()
    {
        // std::cout << "init one table block begin" << std::endl;
        table_amount = 0;
        CalAndUpdateFreeSpace();
        next_block_pointer = 0x0;

        // init tables_begin_address
        tables_begin_address = nullptr;

        // open one memory block, and make data* controlled by mm
        MemoryManagement* mm = MemoryManagement::GetInstance();
        mm->GetFreeTableBlock(data);

        // std::cout << "init one table block end" << std::endl;
    }

    ~TableBlock()
    {
        // std::cout << "delete table block!" << std::endl;
        
        MemoryManagement* mm = MemoryManagement::GetInstance();
        mm->ReleaseBlock(data);

        delete[] tables_begin_address;

        // std::cout << "delete table block end!" << std::endl;
    }
    
    void CalAndUpdateFreeSpace()
    {
        free_space = BLOCK_SIZE;
        free_space -= sizeof(default_amount_type) - sizeof(default_length_size) - sizeof(default_address_type);

        if (table_amount != 0) {
            default_length_size used_space = BLOCK_SIZE;
            used_space -= tables_begin_address[table_amount - 1];
            free_space -= used_space;
        }
    }

    // return true if the address is ok
    // return false when there has no space to contain the table in this block
    bool CalBeginAddress(ColumnTable* table, default_address_type& address)
    {
        if (table_amount == 0)
        {
            address = BLOCK_SIZE;
            address -= table->Serialize().second;
            return true;
        }

        address = tables_begin_address[table_amount - 1] - table->Serialize().second;

        // if this block has no space to contain this table
        if (address >= sizeof(default_amount_type) + sizeof(default_length_size) + sizeof(default_address_type) + table_amount * sizeof(default_address_type))
        {
            return true;
        }

        return false;
    }

    void InsertTable(ColumnTable* table)
    {   
        std::cout << "begin insert table " << std::endl;
        default_address_type insert_address;
        if (table_amount == 0)
        {   
            if (CalBeginAddress(table, insert_address))
            {  
                table_amount++;
                tables_begin_address = new default_address_type[1];
                tables_begin_address[0] = insert_address;
                cout << "insert_address: " << insert_address << endl;
                memcpy(data + insert_address, table->Serialize().first, table->Serialize().second);
                cout << "insert_ data: " << data[insert_address] << data[insert_address + 1] << endl;
                SerializeHeader();
            }
            else
            {
                throw std::runtime_error("Failed to insert table in block");
            }
            std::cout << "insert table end" << std::endl;
            return;
        }

        if (CalBeginAddress(table, insert_address))
        {   
            table_amount++;
            default_address_type* cache_address = new default_address_type[table_amount];
            memcpy(cache_address, tables_begin_address, (table_amount - 2));
            cache_address[table_amount - 1] = insert_address;
            delete[] tables_begin_address;
            tables_begin_address = cache_address;

            // write in memory block
            memcpy(data + insert_address, table->Serialize().first, table->Serialize().second);
        }
        else
        {
            throw std::runtime_error("Failed to insert table in block");
            // TODO: create new table block and insert
        }

        SerializeHeader();
        // std::cout << "insert table end" << std::endl;
    }

    /**
     * @brief Serialize the TableBlock struct to a binary buffer which is controlled by mm.
     */
    void SerializeHeader() {
        size_t offset = 0;

        // Write the table amount
        memcpy(data + offset, &table_amount, sizeof(default_amount_type));
        offset += sizeof(default_amount_type);

        // Write the free space
        memcpy(data + offset, &free_space, sizeof(default_length_size));
        offset += sizeof(default_length_size);

        // Write the next block pointer
        memcpy(data + offset, &next_block_pointer, sizeof(default_address_type));
        offset += sizeof(default_address_type);

        // Write the table begin addresses
        for (default_amount_type i = 0; i < table_amount; ++i) {
            memcpy(data + offset, &tables_begin_address[i], sizeof(default_address_type));
            offset += sizeof(default_address_type);
        }
    }

    /**
     * @brief Deserialize a TableBlock struct from a binary buffer.
     * 
     * @param buffer The binary buffer to read from.
     */
    void DeserializeFromBuffer(const char* buffer) 
    {
        size_t offset = 0;

        // Read the table amount
        memcpy(&table_amount, buffer + offset, sizeof(default_amount_type));
        offset += sizeof(default_amount_type);

        // Read the free space
        memcpy(&free_space, buffer + offset, sizeof(default_length_size));
        offset += sizeof(default_length_size);

        // Read the next block pointer
        memcpy(&next_block_pointer, buffer + offset, sizeof(default_address_type));
        offset += sizeof(default_address_type);

        // Read the table begin addresses
        tables_begin_address = new default_address_type[table_amount];
        for (default_amount_type i = 0; i < table_amount; ++i) {
            memcpy(&tables_begin_address[i], buffer + offset, sizeof(default_address_type));
            offset += sizeof(default_address_type);
        }
    }
};

}

#endif // VDBMS_META_BLOCK_TABLE_BLOCK_H_