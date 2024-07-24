// Copyright (c) 2024 by dingning
//
// file  : table_file_management.h
// since : 2024-07-20
// desc  : TODO.

#ifndef VDBMS_STORAGE_TABLE_FILE_MANAGEMENT_H_
#define VDBMS_STORAGE_TABLE_FILE_MANAGEMENT_H_

#include <string>
#include <iostream>
#include <fstream>


#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "../config.h"

namespace tiny_v_dbms {

using std::string;
using std::fstream;


class TableFileManagement
{
public:

    /**
     * Opens a table file and returns a stream to read from and write to it.
     * 
     * This function checks if the file path has the correct suffix and opens the file
     * in read-write mode. The file stream is not explicitly returned, but it can be used
     * by other functions.
     * 
     * @param file_path The path to the table file to open.
     * 
     * Example:
     * ```cpp
     * string file_path = "example.table";
     * fstream file_stream;
     * OpenDataFile(file_path, file_stream);
     * ```
    */
    void OpenDataFile(string& file_path, fstream& file_stream)
    {   
        std::cout << "OpenDataFile: " << file_path << std::endl;
        // Check if the file path has the correct suffix
        string suffix = TABLE_FILE_SUFFIX;
        if ("." + file_path.substr(file_path.find_last_of(".") + 1) != suffix) {
            throw std::runtime_error("Invalid file suffix. Expected:" + suffix + " but found" + file_path.substr(file_path.find_last_of(".") + 1));
        }

        // Open the file in read-write mode
        file_stream.open(file_path, std::ios::in | std::ios::out);

        // Check if the file was opened successfully
        if (!file_stream.is_open()) {
            throw std::runtime_error("Failed to open file: " + file_path);
        }
    }

    /**
     * Reads a block of data from a file stream.
     * 
     * This function sets the file stream's read pointer to the specified address 
     * (multiplied by the block size) and prepares the stream to read a block of data.
     * 
     * @param file_stream The file stream to read from.
     * @param tables_begin_address The address of the block to read, in units of blocks.
     * 
     * @example
     * ```cpp
     * fstream file_stream("example.bin", ios::in | ios::binary);
     * default_address_type block_address = 5; // read the 5th block
     * GetBlock(file_stream, block_address);
     * char buffer[BLOCK_SIZE];
     * file_stream.read(buffer, BLOCK_SIZE); // read the block into the buffer
     * ```
    */
    void GetBlock(fstream& file_stream, default_address_type block_address)
    {   
        default_length_size block_size = BLOCK_SIZE;
        // check the tables_begin_address is less than file_length / BLOCK_SIZE
        assert(block_address < file_stream.tellg() / block_size);
        
        // set file_stream read pointer to tables_begin_address * BLOCK_SIZE
        file_stream.seekg(block_address * block_size, std::ios::beg);
    }

    /**
     * Returns the address of the next free block in the file stream.
     * 
     * This function returns the current position of the file stream's read pointer 
     * divided by the block size, which represents the address of the next free block.
     * 
     * @param file_stream The file stream to read from.
     * 
     * @return The address of the next free block.
     * 
     * @example
     * ```cpp
     * fstream file_stream("example.bin", ios::out | ios::binary);
     * default_address_type free_block_address = GetFreeBlockAddress(file_stream);
     * ```
    */
    default_address_type GetNewBlockAddress(fstream& file_stream)
    {
        default_length_size block_size = BLOCK_SIZE;
        return file_stream.tellg() / block_size;
    }

    /**
     * Writes a block of data to a file stream.
     * 
     * This function sets the file stream's write pointer to the specified block address 
     * (multiplied by the block size) and writes the provided data to the block.
     * 
     * @param file_stream The file stream to write to.
     * @param block_address The address of the block to write to.
     * @param data The data to write to the block.
     * 
     * @example
     * ```cpp
     * fstream file_stream("example.tvdbb", ios::out | ios::binary);
     * default_address_type block_address = 5; // write to the 5th block
     * char data[BLOCK_SIZE] = {  initialize data  };
     * WriteBackBlock(file_stream, block_address, data);
     * ```
    */
    void WriteBackBlock(fstream& file_stream, default_address_type block_address, char* data)
    {   
        // std::cout << "WriteBackBlock address:" << block_address << std::endl;
        default_length_size block_size = BLOCK_SIZE;
        file_stream.seekg(block_address * block_size, std::ios::beg);
        file_stream.write(data, block_size);
    }

    default_address_type GetFileBlocksAmount(fstream& file_stream)
    {
        return file_stream.tellg() / BLOCK_SIZE;
    }

    void ReadFromFile(fstream& file_stream, default_address_type block_address, char* data)
    {
        // std::cout << "ReadFromFile address:" << block_address << std::endl;
        file_stream.seekg(block_address * BLOCK_SIZE, std::ios::beg);
        file_stream.read(data, BLOCK_SIZE);
    }
private:

};


}

#endif // VDBMS_STORAGE_TABLE_FILE_MANAGEMENT_H_