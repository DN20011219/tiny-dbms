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


using std::string;
using std::cin;
using std::cout;
using std::ios;
// using std::filesystem::current_path;

namespace tiny_v_dbms {

    // get install path from file
    void GetInstallPath(string& install_path) 
    {

        string cache_file_uri = "./install_url.txt";
        std::ifstream file_read;
        file_read.open(cache_file_uri);
        if (!file_read) {
            throw std::runtime_error("Failed to open install file for reading");
        }
        getline(file_read, install_path);
        file_read.close();

    }

    void OpencvDirAndMkdir(const string& folder_path)
    {
        DIR * mydir =NULL;
        mydir=opendir(folder_path.c_str());
        if(mydir==NULL)
        {
            mkdir(folder_path.c_str(), 0777);
        }
        closedir(mydir);
    }


    void CreateBaseDB(const string& install_path) 
    {
        string folder_path = install_path + "/default_db";
        OpencvDirAndMkdir(folder_path);

        string file_path = folder_path + "default_db.tvdb";
        std::ofstream file_write(file_path.c_str());
        if (!file_write) {
            throw std::runtime_error("Failed to create default DB file: " + install_path);
        }
        cout << "default db file created in " << file_path << std::endl;
    }



    void InstallDBMS() 
    {

        string install_path;
        GetInstallPath(install_path);

        if (install_path.length() != 0) {
            cout << "tiny-vector-dbms areadly installed in " << install_path << std::endl;
        }
        else {
            // try install tiny-vector-dbms

            cout << "input the install path (must be a folder): ";
            cin >> install_path;
            cout << "try install tiny-vector-dbms in " << install_path << std::endl;
 
            const int length = install_path.length(); 
            char* install_path_char = new char[length];
            strcpy(install_path_char, install_path.c_str());

            std::ofstream file_write;
            file_write.open(install_path);
            file_write.write(install_path_char, length);
            file_write.close();

            // check install folder exist and empty
            DIR* install_dir = opendir(install_path.c_str());
            if (install_dir == nullptr) {
                throw std::runtime_error("Install path not exist!");
            }

            // TODOTODO
            readdir(install_dir);
            if (readdir(install_dir) != nullptr) {
                throw std::runtime_error("Install path not empty!");
            }

            CreateBaseDB(install_path); // create a default db file.
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