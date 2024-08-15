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
#include "../sql/parser/separater.h"
#include "../sql/parser/parser.h"
// #include "db_management.h"
#include "server.h"

using std::string;
using std::cin;
using std::cout;
using std::ios;
// using std::filesystem::current_path;

namespace tiny_v_dbms {


    // // get install path from file
    // void GetInstallPath(string& install_path) 
    // {
    //     FileManagement file_mm;
    //     string cache_file_uri = INSTALL_PATH_CACHE_FILE;
    //     std::ifstream file_read = file_mm.ReadOrCreateFile(cache_file_uri);
    //     if (!file_read) {
    //         throw std::runtime_error("Failed to open install file for reading");
    //     }
    //     getline(file_read, install_path);
    //     file_read.close();

    // }

    // void WriteInstallPath(string& install_path) 
    // {
    //     string cache_file_uri = INSTALL_PATH_CACHE_FILE;
    //     std::ofstream file_write;
    //     file_write.open(cache_file_uri);
    //     file_write.write(install_path.c_str(), install_path.length());
    //     file_write.close();
    // }

    // void InstallDBMS() 
    // {
    //     // get install path from file
    //     string install_path;
    //     GetInstallPath(install_path);

    //     // empty means not installed
    //     if (install_path.length() != 0) {
    //         cout << "tiny-vector-dbms areadly installed in " << install_path << std::endl;

    //         // install the default db
    //         DBManagement db_mm;
    //         db_mm.CreateSystemDB();

    //         cout << "install successfully in: " << install_path << std::endl;
    //     }
    //     // try install tiny-vector-dbms
    //     else {
    //         cout << "input the install path (must be a folder): ";
    //         cin >> install_path;
    //         cout << "try install tiny-vector-dbms in " << install_path << std::endl;

    //         // write install path to storage file
    //         WriteInstallPath(install_path); 

    //         // install the default db
    //         DBManagement db_mm;
    //         db_mm.CreateSystemDB();

    //         cout << "install successfully in: " << install_path << std::endl;
    //     }
    // }

    // void CreateDB()
    // {
    //     string db_name;
    //     cout << "input the db name: ";
    //     cin >> db_name;
    //     DBManagement db_mm;
    //     db_mm.CreateDB(db_name);
    // }

    // void CreateTable()
    // {
    //     string sql = "USE db CREATE table(column1 VCHAR, column2 NUMBER)"; 
    // }

    // void InsertData()
    // {
    // }

    // void ExecuteSQL()
    // {   
    //     cin.clear();

    //     Parser parser;
    //     NodeBuilder builder;

    //     while(true) {

    //         // input sql
    //         string sql;
    //         getline(cin, sql);
                
    //         // seperate sql x
    //         std::vector<Token> tokens = Tokenize(sql);
    //         cout << "tokens: ";
    //         PrintTokens(tokens);

    //         // parse sql 
    //         NodeType node_type;
    //         try {
    //             node_type = parser.ParseSql(tokens);
    //             cout << "success parse sql! " << node_type << endl;
    //         } catch (std::runtime_error e)
    //         {   
    //             node_type = NodeType::UNSUPPORT_NODE;
    //         }
            
    //         // parse AST
    //         if (node_type != NodeType::UNSUPPORT_NODE) 
    //         {
    //             AST* ast = builder.BuildNode(tokens, node_type);
    //             cout << "Build AST contents: " << ast->ToString() << endl;
    //         }
    //         cout << endl;
            
    //         cin.clear();
    //     }
    // }

    void StartBench() 
    {
        Server server;
        server.Run();

        // ExecuteSQL();

        // char ctr;
        // while(true) {
        //     cin >> ctr;
        //     switch (ctr)
        //     {
        //     case 'Q': // quit
        //         return;
        //     case 'I': // install
        //         InstallDBMS();
        //         break;
        //     case 'C': // create
        //         CreateDB();
        //         break;
        //     case 'S': // add
        //         ExecuteSQL();
        //         break;
        //     default:
        //         cout << "default" << std::endl;
        //         break;
        //     }
        // }
    }

    


}

#endif // VDBMS_BENCH_DEMO_BENCH_H_