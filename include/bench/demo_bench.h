// Copyright (c) 2024 by dingning
//
// file  : demo_bench.h
// since : 2024-07-21
// desc  : TODO.

#ifndef VDBMS_BENCH_DEMO_BENCH_H_
#define VDBMS_BENCH_DEMO_BENCH_H_

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "../storage/file_management.h"

using std::string;
using std::cin;
using std::cout;
using std::ios;
// using std::filesystem::current_path;

namespace tiny_v_dbms {



    // get install path from file
    void GetInstallPath(string& install_path) 
    {
        FileManagement file_mm;
        string cache_file_uri = "./install_url.txt";
        std::ifstream file_read = file_mm.OpenOrCreateFile(cache_file_uri);
        if (!file_read) {
            throw std::runtime_error("Failed to open install file for reading");
        }
        getline(file_read, install_path);
        file_read.close();

    }

    void WriteInstallPath(string& install_path) 
    {
        string cache_file_uri = "./install_url.txt";
        std::ofstream file_write;
        file_write.open(cache_file_uri);
        file_write.write(install_path.c_str(), install_path.length());
        file_write.close();
    }

    // create default db folder and file
    void CreateBaseDB(const string& install_path) 
    {   
        FileManagement file_mm;

        // create folder
        string folder_path = install_path + "/default_db";
        file_mm.OpencvDirAndMkdir(folder_path);

        // create db file
        string file_path = folder_path + "/" + "default_db.tvdb";
        string mock_write_data = "install_success"; // TODO:change it to a real db file
        const char* data_pointer = mock_write_data.c_str();
        file_mm.WriteFile(file_path, data_pointer, mock_write_data.length());
    }

    void InstallDBMS() 
    {
        // get install path from file
        string install_path;
        GetInstallPath(install_path);

        // empty means not installed
        if (install_path.length() != 0) {
            cout << "tiny-vector-dbms areadly installed in " << install_path << std::endl;
        }
        // try install tiny-vector-dbms
        else {
            cout << "input the install path (must be a folder): ";
            cin >> install_path;
            cout << "try install tiny-vector-dbms in " << install_path << std::endl;
            
            // check install folder exist and empty
            DIR* install_dir = opendir(install_path.c_str());
            FileManagement file_mm;
            if (install_dir == nullptr) {
                throw std::runtime_error("Install path not exist!");
            }
            file_mm.IsEmptyDir(install_path);

            CreateBaseDB(install_path); // create a default db file.
            WriteInstallPath(install_path); // write install path to storage file

            cout << "install successfully" << install_path << std::endl;
        }
    }

    int StartBench() 
    {

        char ctr;
        while(true) {
            cin >> ctr;
            switch (ctr)
            {
            case 'Q': // quit
                return 0;
            case 'T': // test
                cout << "test" << std::endl;
                break;
            case 'I': // install
                InstallDBMS();
                break;
            default:
                cout << "default" << std::endl;
                break;
            }
        }

    }

 
}

#endif // VDBMS_BENCH_DEMO_BENCH_H_