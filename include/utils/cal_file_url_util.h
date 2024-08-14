// Copyright (c) 2024 by dingning
//
// file  : cal_file_url_util.h
// since : 2024-08-08
// desc  : This is a util to cal file url using db_name, table_name.

#ifndef UTILS_CAL_FILE_URL_UTIL_H_
#define UTILS_CAL_FILE_URL_UTIL_H_

#include <string>

#include "../config.h"
#include "../storage/file_management.h"

namespace tiny_v_dbms {

class CalFileUrlUtil
{

private:
    static std::string install_path;

public:

    CalFileUrlUtil()
    {
        GetInstallPathFromDisk();
    }

    // get install path from file and store in cache
    void GetInstallPathFromDisk() 
    {
        FileManagement* file_mm = new FileManagement();
        std::ifstream file_read = file_mm->ReadOrCreateFile(INSTALL_PATH_CACHE_FILE);
        getline(file_read, install_path);
        file_read.close();
        delete file_mm;

        if (install_path == "")
        {
            throw std::runtime_error("dbms has not been installed!");
        }
    }

    // get install path from cache
    static string GetInstallPath() 
    {
        return install_path;
    }

    /**
     * @brief Get the folder where installed base db,
     * @return The folder where installed base db.
     */ 
    static string GetBaseDbFolder()
    {
        return GetDbFolder(DEFAULT_DB_FOLDER_NAME);
    }

    /**
     * @brief Get the folder where installed input db. Read base install path from file
     * @param db_name db name
     * @return  The folder where installed input db.
     */
    static string GetDbFolder(string db_name)
    {
        return GetInstallPath() + "/" + db_name;;
    }

    /**
     * @brief Get the db file of input db.
     * @param db_name db name
     * @return  The path where is he db file of input db.
     */
    static string GetDefaultDbFile(string db_name)
    {   
        return GetDbFolder(db_name) + "/" + db_name + DB_FILE_SUFFIX;
    }

    static string GetDefaultTablePath(string db_name)
    {   
        return GetDbFolder(db_name) + "/" + DEFAULT_TABLE_FOLDER;
    }

    static string GetDefaultTableFile(string db_name)
    {   
        return GetDefaultTablePath(db_name) + "/" + DEFAULT_TABLE_NAME + TABLE_FILE_SUFFIX;
    }

    static string GetTableHeaderFile(string db_name, string table_name)
    {
        return GetDefaultTablePath(db_name) + "/" + table_name + TABLE_FILE_SUFFIX;
    }
    static string GetTableDataFile(string db_name, string table_name)
    {
        return GetDefaultTablePath(db_name) + "/" + DEFAULT_TABLE_DATA_FOLDER + "/" + table_name + TABLE_DATA_FILE_SUFFIX;
    }
};

}

#endif // UTILS_CAL_FILE_URL_UTIL_H_