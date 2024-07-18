// Copyright (c) 2024 by dingning
//
// file  : file_management.h
// since : 2024-07-18
// desc  : 

#ifndef VDBMS_STORAGE_FILE_MANAGEMENT_H_
#define VDBMS_STORAGE_FILE_MANAGEMENT_H_

#include <string>
#include <fstream>

#include "../config.h" 

namespace tiny_v_dbms {

using std::string; 
using std::ios;

class FileManagement 
{
public:
    
    void ReadFile(string& file_dir, char* memory_pointer) {
        std::ifstream file(file_dir, ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to open file for reading");
        }
        file.seekg(0, ios::end);
        size_t file_size = file.tellg();
        file.seekg(0, ios::beg);
        file.read(memory_pointer, file_size);
        file.close();
    }

    void WriteFile(string& file_dir, char* memory_pointer, default_length_size data_size) {
        std::ofstream file(file_dir, ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to open file for writing");
        }
        file.write(memory_pointer, data_size);
        file.close();
    }

private:
    
};

}

#endif // VDBMS_STORAGE_FILE_MANAGEMENT_H_