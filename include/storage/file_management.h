// Copyright (c) 2024 by dingning
//
// file  : file_management.h
// since : 2024-07-18
// desc  : 

#ifndef VDBMS_STORAGE_FILE_MANAGEMENT_H_
#define VDBMS_STORAGE_FILE_MANAGEMENT_H_

#include <string>
#include <fstream>

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "../config.h" 

namespace tiny_v_dbms {

using std::string; 
using std::ios;

class FileManagement 
{
public:
    
    // not used read
    default_length_size ReadFile(string file_dir, char* memory_pointer) {
        std::ifstream file(file_dir, ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to open file for reading");
        }
        file.seekg(0, ios::end);
        size_t file_size = file.tellg();
        file.seekg(0, ios::beg);
        file.read(memory_pointer, file_size);
        file.close();

        return file_size;
    }

    void WriteFile(string file_dir, const char* memory_pointer, default_length_size data_size) {
        std::ofstream file(file_dir, ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to open file for writing");
        }
        file.write(memory_pointer, data_size);
        file.close();
    }

    /**
     * Appends data from memory to a file.
     * 
     * @param file_dir The directory path and filename to append to.
     * @param memory_pointer A pointer to the memory location containing the data to append.
     * @param data_size The size of the data to append in bytes.
     * 
     * @throws std::runtime_error If the file cannot be opened for appending.
     * 
     * Example:
     * ```cpp
     * int data[] = {1, 2, 3, 4, 5};
     * WriteFile("example.bin", reinterpret_cast<const char*>(data), sizeof(data));
     * ```
     * This example appends the contents of the `data` array to a file named "example.bin".
     */
    void WriteFileAppend(string& file_dir, const char* memory_pointer, default_length_size data_size) {
        std::ofstream file(file_dir, ios::binary | ios::app);
        if (!file) {
            throw std::runtime_error("Failed to open file for appending");
        }
        file.write(memory_pointer, data_size);
        file.close();
    }

    // open a read stream to input file_path
    // if file not exist, create one and open it
    std::ifstream ReadOrCreateFile(const string file_path)
    {
        std::ifstream file_write;

        file_write.open(file_path, ios::binary);
        
        if (!file_write || !file_write.is_open())
        {
            std::ofstream fout(file_path);
            fout.close();
            file_write.open(file_path, ios::binary);
        }
        
        if (!file_write || !file_write.is_open())
            throw std::runtime_error("Failed to create file:" + file_path);

        return file_write;
    }

    void OpenOrMkdir(const string& folder_path)
    {
        DIR * mydir =NULL;
        mydir=opendir(folder_path.c_str());
        if(mydir==NULL)
        {
            mkdir(folder_path.c_str(), 0755);
        }
        else
        {
            closedir(mydir);
        }
    }

    void IsEmptyDir(std::string folderPath)
    {
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir(folderPath.c_str())) != NULL) 
        {
            while ((ent = readdir(dir)) != NULL) 
            {
                std::string filename(ent->d_name);
                if (filename != "." && filename != "..")
                    throw std::runtime_error("Install path not empty!");
            }
            closedir(dir);
        }
    }

private:
    
};

}

#endif // VDBMS_STORAGE_FILE_MANAGEMENT_H_