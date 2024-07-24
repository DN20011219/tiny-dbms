// Copyright (c) 2024 by dingning
//
// file  : data_block.h
// since : 2024-07-18
// desc  : Basic unit to read from disk or write to disk. This type block means 
// each block only store one type of field (same named fields from different t-
// ables are regarded as different fields), and data is stored one besides ano-
// ther one.
// Beside, all blocks of one table will be stored in one file. use the block nu-
// mber to locate the block.

/*

| address |  data description |

0 ---------block begin 4kb----------------------
0 | field length (20) |
4 | field_data_nums (1)|
8 | pre_block_pointer (0x0000) |
12| next_block_pointer (0x0000) |

4075 | field data (contains 20 byte data)|
4096 ------block end----------------------------

*/

#ifndef VDBMS_META_BLOCK_DATA_BLOCK_H_
#define VDBMS_META_BLOCK_DATA_BLOCK_H_

#include <string>

#include "../../config.h" 
#include "../../memory/memory_management.h"


namespace tiny_v_dbms {

using std::string; 

struct DataBlock 
{

public:

    // information about this block
    default_length_size field_length;           // length of per field
    default_length_size field_data_nums;        // amount of data
    default_address_type pre_block_pointer;     // store a pointer to pre block, if has pre block
    default_address_type next_block_pointer;    // store a pointer to next block, if has next block

    // not serialize field
    char* data;                                 // data pointer in memory, used to visit memory
    
    DataBlock() {
        assert(default_pointer_size == sizeof(next_block_pointer));     // must be 32bit device to work
        
        field_length = 0;
        field_data_nums = 0;

        // open one memory block, and make data* controlled by mm
        MemoryManagement* mm = MemoryManagement::GetInstance();
        mm->GetFreeTableBlock(data);
    }

    ~DataBlock()
    {
        std::cout << "delete data block!" << std::endl;
        
        MemoryManagement* mm = MemoryManagement::GetInstance();
        mm->ReleaseBlock(data);

        std::cout << "delete data block end!" << std::endl;
    }

    // return true if the address is ok
    // return false when there has no space to contain the data in this block
    bool CalBeginAddress(default_address_type& address)
    {   
        address = BLOCK_SIZE;
        address -= field_data_nums * field_length;

        // if this block has no space to contain this table
        if (address < 2 * sizeof(default_length_size) + 2 * sizeof(default_address_type))
        {
            return false;
        }
        return true;
    }

    void InsertData(char* insert_data)
    {   
        field_data_nums++;
        default_address_type address;
        if (CalBeginAddress(address))
        {   
            memcpy(data + address, insert_data, field_length);
        }
        else{
            throw std::runtime_error("Failed to insert data in block, need more space");
        }
    }

    // Serialization method
    void Serialize() const {
        size_t offset = 0;

        memcpy(data + offset, &field_length, sizeof(default_length_size));
        offset += sizeof(default_length_size);

        memcpy(data + offset, &field_data_nums, sizeof(default_length_size));
        offset += sizeof(default_length_size);

        memcpy(data + offset, &pre_block_pointer, sizeof(default_address_type));
        offset += sizeof(default_address_type);

        memcpy(data + offset, &next_block_pointer, sizeof(default_address_type));
    } 

    /**
     * @brief Deserialize a DataBlock struct from a binary buffer.
     * 
     * @param buffer The binary buffer to read from.
     */
    void DeserializeFromBuffer(const char* buffer) 
    {
        size_t offset = 0;

        // Read the field_length
        memcpy(&field_length, buffer + offset, sizeof(default_length_size));
        offset += sizeof(default_length_size);

        // Read the field_data_nums
        memcpy(&field_data_nums, buffer + offset, sizeof(default_length_size));
        offset += sizeof(default_length_size);

        // Read the pre block pointer
        memcpy(&pre_block_pointer, buffer + offset, sizeof(default_address_type));
        offset += sizeof(default_address_type);

        // Read the next block pointer
        memcpy(&next_block_pointer, buffer + offset, sizeof(default_address_type));

    }

    default_length_size GetSpaceCost()
    {
        return 2 * sizeof(default_length_size) + 2 * sizeof(default_address_type) + field_data_nums * field_length;
    }

    bool HaveSpace()
    {
        if (BLOCK_SIZE - GetSpaceCost() > field_length)
        {
            return true;
        }
        return false;
    }

};

}

#endif // VDBMS_META_BLOCK_DATA_BLOCK_H_