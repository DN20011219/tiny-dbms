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
8 | last record start address (4075) |
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

class DataBlock 
{

public:

    // information about this block
    default_length_size field_length;           // length of per field
    default_length_size field_data_nums;        // amount of data
    default_address_type last_record_start_address;     // last record start address, use to cal free space
    default_address_type next_block_pointer;    // store a pointer to next block, if has next block

    // not serialize field
    char* data;                                 // data pointer in memory, used to visit memory
    
    DataBlock() {
        assert(default_pointer_size == sizeof(next_block_pointer));     // must be 32bit device to work
        
        field_length = 0;
        field_data_nums = 0;
    }

    ~DataBlock()
    {
    }

    void InitBlock(default_length_size value_length)
    {
        field_length = value_length;
        field_data_nums = 0;
        last_record_start_address = BLOCK_SIZE;
        next_block_pointer = 0x0;
    }

    // return true if the address is ok
    // return false when there has no space to contain the data in this block
    bool CalBeginAddress(default_address_type& address, default_length_size data_size)
    {   
        address = last_record_start_address - data_size;

        // if this block has no space to contain this table
        if (address < 2 * sizeof(default_length_size) + 2 * sizeof(default_address_type))
        {
            return false;
        }
        return true;
    }

    void InsertData(char* insert_data, default_length_size data_size)
    {   
        field_data_nums++;
        default_address_type address;
        if (CalBeginAddress(address, data_size))
        {   
            memcpy(data + address, insert_data, data_size);
            last_record_start_address = address;
        }
        else{
            throw std::runtime_error("Failed to insert data in block, need more space");
        }
    }

    void DeleteData(default_address_type data_address)
    {
        // check data is in address array, if not throw error
        if (!CheckDataExist(data_address))
        {
            throw std::runtime_error("data address is not in block, but try delete it");
        }

        default_address_type start_address = BLOCK_SIZE - field_data_nums * field_length;
        if (start_address == data_address) 
        {
            // no need to move
        }
        else{
            // move [start_address, data_address - 1] to [start_address + field_length, data_address - 1 + field_length]
            // to earse data at data_address
            MoveData(start_address, data_address - 1, field_length);
        }
        
        field_data_nums--;
        Serialize();
    }

    bool CheckDataExist(default_address_type data_address)
    {
        // check address is on data range and length
        if (data_address < BLOCK_SIZE - field_length * field_data_nums)
        {
            return false;
        }

        if ((BLOCK_SIZE - data_address + 1) % field_length != 0)
        {
            return false;
        }

        return true;
    }

    // move [segment_begin, segment_end] offset byte right
    void MoveData(default_address_type segment_begin, default_address_type segment_end, default_address_type offset)
    {   
        default_address_type copy_address = segment_end + offset;
        while (segment_end >= segment_begin)
        {
            memcpy(data + segment_end, data + copy_address, 1);
            segment_end--;
            copy_address--;
        }
    }

    // Serialization method
    void Serialize() const {
        size_t offset = 0;

        memcpy(data + offset, &field_length, sizeof(default_length_size));
        offset += sizeof(default_length_size);

        memcpy(data + offset, &field_data_nums, sizeof(default_length_size));
        offset += sizeof(default_length_size);

        memcpy(data + offset, &last_record_start_address, sizeof(default_address_type));
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

        // Read the last_record_start_address
        memcpy(&last_record_start_address, buffer + offset, sizeof(default_address_type));
        offset += sizeof(default_address_type);

        // Read the next block pointer
        memcpy(&next_block_pointer, buffer + offset, sizeof(default_address_type));

    }

    default_length_size GetSpaceCost()
    {
        return 2 * sizeof(default_length_size) + 2 * sizeof(default_address_type) + (BLOCK_SIZE - last_record_start_address);
    }

    bool HaveSpace(default_length_size value_length)
    {
        if ((BLOCK_SIZE - GetSpaceCost()) > value_length)
        {
            return true;
        }
        return false;
    }

};

}

#endif // VDBMS_META_BLOCK_DATA_BLOCK_H_