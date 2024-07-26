// Copyright (c) 2024 by dingning
//
// file  : db_management.h
// since : 2024-07-19
// desc  : 
/*
install folder
--db_name
----


*/

#ifndef VDBMS_BENCH_DB_MANAGEMENT_H_
#define VDBMS_BENCH_DB_MANAGEMENT_H_

#include <iostream>

#include "../config.h"
#include "../meta/db/db.h"
#include "../storage/file_management.h"
#include "../memory/memory_management.h"
#include "../meta/table/column_table.h"
#include "../meta/block/table_block.h"
#include "../meta/block/data_block.h"
#include "../storage/block_file_management.h"

namespace tiny_v_dbms {


class DBManagement {
private:
    FileManagement* file_mm;
    BlockFileManagement* bfmm;

    // get install path from file
    string GetInstallPath() 
    {
        string install_path;

        std::ifstream file_read = file_mm->ReadOrCreateFile(INSTALL_PATH_CACHE_FILE);
        getline(file_read, install_path);
        file_read.close();

        return install_path;
    }

    /**
     * @brief Get the folder where installed base db,
     * @return The folder where installed base db.
     */ 
    string GetBaseDbFolder()
    {
        return GetAndCreateDbFolder(DEFAULT_DB_FOLDER_NAME);
    }

    /**
     * @brief Get the folder where installed input db. Read base install path from file
     * @param db_name db name
     * @return  The folder where installed input db.
     */
    string GetAndCreateDbFolder(string db_name)
    {
        if (GetInstallPath() == "")
        {
            throw std::runtime_error("dbms has not been installed!");
        }
        string db_folder = GetInstallPath() + "/" + db_name;
        file_mm->OpencvDirAndMkdir(db_folder);
        return db_folder;
    }

    /**
     * @brief Get the db file of input db.
     * @param db_name db name
     * @return  The path where is he db file of input db.
     */
    string GetAndCreateDefaultDbFile(string db_name)
    {   
        // create db file, db file has the same name as db folder
        string db_file_name = GetAndCreateDbFolder(db_name) + "/" + db_name + DB_FILE_SUFFIX;
        file_mm->ReadOrCreateFile(db_file_name).close();
        return db_file_name;
    }

    /**
     * @brief Cal and return the default table path base on the input db path.
     * @param db_path db folder path
     * @return default table path of the input db path.
     */
    string GetDefaultTablePath(string db_path)
    {   
        // create table folder
        string default_table_folder = db_path + "/" + DEFAULT_TABLE_FOLDER;
        file_mm->OpencvDirAndMkdir(default_table_folder);
        return default_table_folder;
    }

    /**
     * @brief Cal and return the default table header file path base on the input tables path.
     * @param tables_path tables folder path, like: install_path/base_db/tables
     * @return default table path of the input db path.
     */
    string GetAndCreateDefaultTableFile(string tables_path)
    {
        string default_table_header_uri = tables_path + "/" + DEFAULT_TABLE_NAME + TABLE_FILE_SUFFIX;
        file_mm->ReadOrCreateFile(default_table_header_uri).close();
        return default_table_header_uri;
    }

    /**
     * Reads a table block from a file
     * @param table_file_uri The URI of the table file
     * @param offset The offset of the block in the file
     * @param new_block The TableBlock object to store the read data
    */
    void ReadOneTableBlock(string table_file_uri, default_address_type offset, TableBlock& new_block)
    {
        fstream file_stream;
        bfmm->OpenTableFile(table_file_uri, file_stream);    // open table header file, like "test.tvdbb"
        bfmm->ReadFromFile(file_stream, offset, new_block.data);
        new_block.DeserializeFromBuffer(new_block.data);
        file_stream.close();
    }

    /**
     * Reads a data block from a file
     * @param data_file_uri The URI of the data file
     * @param offset The offset of the block in the file
     * @param new_block The DataBlock object to store the read data
    */
    void ReadOneDataBlock(string data_file_uri, default_address_type offset, DataBlock& new_block)
    {
        fstream file_stream;
        bfmm->OpenDataFile(data_file_uri, file_stream);    // open table header file, like "test.data"
        bfmm->ReadFromFile(file_stream, offset, new_block.data);
        new_block.DeserializeFromBuffer(new_block.data);
        file_stream.close();
    }

    void WriteBackTableBlock(string table_file_uri, default_address_type offset, TableBlock& block)
    {
        fstream file_stream;
        bfmm->OpenTableFile(table_file_uri, file_stream);   // open table header file, like "test.tvdbb"   
        bfmm->WriteBackBlock(file_stream, offset, block.data);
        file_stream.close();
    }

    void WriteBackDataBlock(string data_file_uri, default_address_type offset, DataBlock& block)
    {
        block.Serialize();
        fstream file_stream;
        bfmm->OpenDataFile(data_file_uri, file_stream);    // open data file, like "test.data"   
        bfmm->WriteBackBlock(file_stream, offset, block.data);
        file_stream.close();
    }

public:

    /*
        CREATE DATABASE base_db;
        USE base_db
        CREATE TABLE default_table (
            db_names VCHAR(20)
        );
    */
    void CreateSystemDB()
    {   
        cout << "----begin Create Base DB----" << endl;
        CreateBaseDB();

        cout << "----begin init default table----" << endl;
        CreateDefaultTableForBaseDb();
    }
    
    /**
     * @brief Create a new database, for sql: CREATE DATABASE db_name;
     * @param db_name The name of the database to be created
     */
    void CreateDB(string db_name)
    {
        // 1、get or create db folder and file
        string db_path_uri = GetAndCreateDbFolder(db_name);
        string db_file_uri = GetAndCreateDefaultDbFile(db_name);

        DB default_db;
        default_db.db_name = db_name;
        default_db.db_all_tables_path = GetAndCreateDefaultTableFile(GetDefaultTablePath(db_path_uri));
        SerializeDBFile(default_db, db_file_uri);

        // 2、insert one row in base_db.default_table, means add one db
        char* insert_db_name = new char[VCHAR_LENGTH];
        memcpy(insert_db_name, &db_name, VCHAR_LENGTH);
        InsertIntoTable(DEFAULT_DB_FOLDER_NAME, DEFAULT_TABLE_NAME, DEFAULT_TABLE_COLUMN_NAME_ONE, insert_db_name);

        // 3、Create default table
        CreateDefaultTable(db_name);
    }

    // create base db of this system, it will store all db names. so you can search db using it.
    void CreateBaseDB()
    {
        // 1、create db folder
        string db_path_uri = GetBaseDbFolder();

        // 2、create db header file
        string db_file_uri = GetAndCreateDefaultDbFile(DEFAULT_DB_FOLDER_NAME);

        // 3、write data into db header file
        DB default_db;
        default_db.db_name = DEFAULT_DB_FILE_NAME;
        default_db.db_description = "The base db of this dbms, has a table, which stores all db about this dbms";
        default_db.db_all_tables_path = GetAndCreateDefaultTableFile(GetDefaultTablePath(db_path_uri));
        SerializeDBFile(default_db, db_file_uri);
    }

    // for sql : USE db_name CREATE TABLE table_name(column1, column2, ...)
    void CreateTable(string db_name, ColumnTable table)
    {
        // 1、check db exist
        if (!CheckDbExist(db_name))
        {
            throw std::runtime_error(db_name + " not exist");
        }

        // 2、get the default table of db
        ColumnTable default_table;
        GetDefaultTable(db_name, default_table);

        // 3、insert one row in it
        char* insert_db_name = new char[VCHAR_LENGTH];
        memcpy(insert_db_name, &table.table_name, VCHAR_LENGTH);
        InsertIntoTable(db_name, table.table_name, DEFAULT_TABLE_COLUMN_NAME_TWO, insert_db_name);
    
        // 4、create table header, 
        string table_header_file = GetAndCreateDefaultTableFile(db_name);
        TableBlock block;
        // here need to found one empty block to write.
        default_address_type block_address = 0;
        ReadOneTableBlock(table_header_file, block_address, block);
        block.DeserializeFromBuffer(block.data);
        while (!block.InsertTable(&table))
        {
            if (block.next_block_pointer == 0 && block_address != 0)
            {
                // table is not too big, big than one block
                throw std::runtime_error(table.table_name + " is too big to insert");
            }
            if (block.next_block_pointer == 0) 
            {
                block_address = bfmm->GetNewBlockAddress(table_header_file);
                block.next_block_pointer = block_address;
            }
            ReadOneTableBlock(table_header_file, block.next_block_pointer, block);
            block.DeserializeFromBuffer(block.data);
        }
        WriteBackTableBlock(table_header_file, block_address, block);

        // 5、create table data file
        string data_file = GetAndCreateDataFile(GetAndCreateDbFolder(db_name), table.table_name);

        // 6、create data block for each column, and write address to table header
        for (default_amount_type i = 0; i < table.column_size; i++)
        {
            default_address_type new_data_block_address = bfmm->GetNewBlockAddress(data_file);
            DataBlock data_block;
            ReadOneDataBlock(data_file, new_data_block_address, data_block);
            data_block.field_length = table.columns.column_length_array[i];
            data_block.field_data_nums = 0;
            data_block.next_block_pointer = 0;
            WriteBackDataBlock(table_header_file, block_address, data_block);
            table.columns.column_storage_address_array[i] = new_data_block_address;
        }
    }
    
    // this is the function is used when creating user table
    void CreateDefaultTable(string db_name)
    {
        // 1、create default table header file
        string tables_path = GetDefaultTablePath(GetAndCreateDbFolder(db_name));
        string default_table_file_uri = GetAndCreateDefaultTableFile(tables_path);

        // 2、write data into table header file (data address is 0, because the data of default table must in the first block of table data file)
        // construct a block to read from or write to disk, construct a column_table(only has one column), and insert to block
        TableBlock block;

        ColumnTable ct;
        ct.table_name = DEFAULT_TABLE_NAME;
        ct.table_type = COMMON;
        ct.InsertColumn(DEFAULT_TABLE_COLUMN_NAME_TWO, VCHAR, VCHAR_LENGTH, NONE, 0);    // insert one column header, column name is db_names
        block.InsertTable(&ct);    // insert table header data to block

        // write back
        fstream file_stream;
        bfmm->OpenTableFile(default_table_file_uri, file_stream);    // open table header file, like "test.tvdbb"
        default_address_type free_address = bfmm->GetNewBlockAddress(file_stream);    // get free block address in file
        bfmm->WriteBackBlock(file_stream, free_address, block.data);
        file_stream.close();

        // 3、store one row, which is "default_table", means the first table is default_table
        string table_name = DEFAULT_TABLE_NAME;
        char* deafult_db_name = new char[ct.columns.column_length_array[ct.column_size - 1]];
        strcpy(deafult_db_name, table_name.c_str());
        
        // 3、INSERT INTO default_table(db_names) VALUES (data) FROM base_db
        InsertIntoTable(DEFAULT_DB_FOLDER_NAME, DEFAULT_TABLE_NAME, DEFAULT_TABLE_COLUMN_NAME_ONE, deafult_db_name);

        free(deafult_db_name);
    }
    
    // create a special default table for base db, this table will store all db's name.
    void CreateDefaultTableForBaseDb()
    {
        // 1、create default table header file
        string tables_path = GetDefaultTablePath(GetBaseDbFolder());
        string default_table_file_uri = GetAndCreateDefaultTableFile(tables_path);

        // 2、write data into table header file (data address is 0, because the data of default table must in the first block of table data file)
        // construct a block to read from or write to disk, construct a column_table(only has one column), and insert to block
        TableBlock block;
        ColumnTable ct;
        ct.table_name = DEFAULT_TABLE_NAME;
        ct.table_type = COMMON;
        ct.InsertColumn(DEFAULT_TABLE_COLUMN_NAME_ONE, VCHAR, VCHAR_LENGTH, NONE, 0);    // insert one column header, column name is db_names
        block.InsertTable(&ct);    // insert table header data to block

        // write back
        fstream file_stream;
        bfmm->OpenTableFile(default_table_file_uri, file_stream);    // open table header file, like "test.tvdbb"
        // default_address_type free_address = bfmm->GetNewBlockAddress(file_stream);    // get free block address in file
        bfmm->WriteBackBlock(file_stream, 0, block.data);
        file_stream.close();
        // cout << "table amount in block is: " << (default_amount_type)block.data[0] << endl;

        // Test deserialize from file: read from file and deserialize, this default block is at 0 offset
        // TableBlock new_block;
        // bfmm->ReadFromFile(file_stream, 0, new_block.data);
        // new_block.DeserializeFromBuffer(new_block.data);
        // assert(new_block.table_amount == 1);
        // assert(new_block.tables_begin_address[new_block.table_amount - 1] == 4008);

        // ColumnTable new_ct;
        // new_ct.Deserialize(new_block.data, new_block.tables_begin_address[new_block.table_amount - 1]);
        // assert(new_ct.column_size == 1);
        // assert(new_ct.columns.column_name_array[new_ct.column_size - 1] == DEFAULT_TABLE_COLUMN_NAME_ONE);
        // assert(new_ct.columns.column_storage_address_array[new_ct.column_size - 1] == 0);

        
        // 3、INSERT INTO default_table(db_names) VALUES (data) FROM base_db
        string db_name = DEFAULT_DB_FOLDER_NAME;
        char* deafult_db_name = new char[ct.columns.column_length_array[ct.column_size - 1]];
        strcpy(deafult_db_name, db_name.c_str());
        InsertIntoTable(DEFAULT_DB_FOLDER_NAME, DEFAULT_TABLE_NAME, DEFAULT_TABLE_COLUMN_NAME_ONE, deafult_db_name);
        free(deafult_db_name);

        // // Test: USE  DEFAULT_TABLE_NAME  SELECT  DEFAULT_TABLE_COLUMN_NAME_ONE  FROM  DEFAULT_TABLE_NAME
        // DataBlock data_block;
        // SelectFromTable(DEFAULT_DB_FOLDER_NAME, DEFAULT_TABLE_NAME, DEFAULT_TABLE_COLUMN_NAME_ONE, data_block);
        // data_block.DeserializeFromBuffer(data_block.data);
        // assert(data_block.field_data_nums == 1);
        // assert(data_block.data[4046] == 'b');
        // assert(data_block.data[4047] == 'a');
        // assert(data_block.data[4048] == 's');
        // assert(data_block.data[4049] == 'e');

    }

    // get the default table of input db
    void GetDefaultTable(string db_name, ColumnTable& default_table)
    {
        if (!CheckTableExist(db_name, DEFAULT_TABLE_NAME, default_table))
        {
            throw std::runtime_error("db_name 's default_table not exist");
        }
    }

    // mainly used when insert data to default table of base db, because the default table of base db will store all db names but not 
    void InsertIntoTableWithoutCheck(string db_name, string table_name, string column_name, char* data)
    {
        
    }

    // for sql: INSERT INTO table_name(column_name) VALUES (data) ;
    void InsertIntoTable(string db_name, string table_name, string column_name, char* data)
    {   
        // check table and column exists.
        ColumnTable table;
        if (!CheckTableExist(db_name, table_name, table)) 
        {
            throw std::runtime_error("table: " + table_name + " not exist in db: " + db_name);
        }

        Columns column;
        if (!CheckColumnExist(table, column_name, column)) 
        {
            throw std::runtime_error("column: " + column_name + "not exists in table: " + table_name);
        }

        // open a data block
        DataBlock data_block;
        string data_file_uri = GetAndCreateDataFile(GetAndCreateDataFolderPath(GetAndCreateDbFolder(db_name)), table_name);

        default_address_type read_offset = column.column_storage_address_array[0];
        ReadOneDataBlock(data_file_uri, read_offset, data_block);

        // get one data block has space
        while (!data_block.HaveSpace())
        {   
            read_offset = data_block.next_block_pointer;

            // if has no next block, then create one new block to store
            if (read_offset == 0x0)
            {
                read_offset = bfmm->GetNewBlockAddress(data_file_uri);
            }

            ReadOneDataBlock(data_file_uri, read_offset, data_block);
        }

        // insert data
        data_block.field_length = column.column_length_array[0];
        data_block.next_block_pointer = 0x0;    // must insert at last block, so set it's next_block_pointer as 0
        data_block.InsertData(data);

        // write back
        WriteBackDataBlock(data_file_uri, read_offset, data_block);
    }

    // for sql: SELECT column_name FROM table_name
    void SelectFromTable(string db_name, string table_name, string column_name, DataBlock& block)
    {
        // check table and column exists.
        ColumnTable table;
        if (!CheckTableExist(db_name, table_name, table))
        {
            throw std::runtime_error("table: " + table_name + " not exist in db: " + db_name);
        }

        Columns column;
        if (!CheckColumnExist(table, column_name, column)) 
        {
            throw std::runtime_error("column: " + column_name + "not exists in table: " + table_name);
        }

        string data_file_uri = GetAndCreateDataFile(GetAndCreateDataFolderPath(GetAndCreateDbFolder(db_name)), table_name);
        default_address_type read_offset = column.column_storage_address_array[0];
        ReadOneDataBlock(data_file_uri, read_offset, block);
    }

    /**
     * @brief Checks if a database exists
     * @param db_name The name of the database to check
     * @return True if the database exists, false otherwise
    */
    bool CheckDbExist(string db_name)
    {
        // get default table of base db
        ColumnTable default_table;
        GetDefaultTable(DEFAULT_DB_FOLDER_NAME, default_table);

        // check whether input db_name in it
        for (default_amount_type i = 0; i < default_table.column_size; i++)
        {
            if (default_table.columns.column_name_array[i] == db_name)
            {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Checks if a table exists in a database.
     *
     * @param db_name The name of the database.
     * @param table_name The name of the table to check.
     * @param table A reference to a ColumnTable object that will be populated with the table's metadata if it exists.
     *
     * @return True if the table exists, false otherwise.
     *
     * @example
     * ColumnTable table;
     * if (CheckTableExist("my_database", "my_table", table)) {
     *     std::cout << "Table exists!" << std::endl;
     * } else {
     *     std::cout << "Table does not exist." << std::endl;
     * }
    */
    bool CheckTableExist(string db_name, string table_name, ColumnTable& table)
    {
        // open db file, like "db.tvbb", and deserialize to get default table name
        string db_file_name = GetAndCreateDbFolder(db_name) + "/"+ db_name + DB_FILE_SUFFIX;

        DB db_file;
        DeserializeDBFile(db_file, db_file_name);

        // open default table file, like "default_table.tvdbb"
        TableBlock block;

        // read db headers file to block, offset is 0, because the first block must at 0 offset
        ReadOneTableBlock(db_file.db_all_tables_path, 0, block);
        block.DeserializeFromBuffer(block.data);
        // deserialize the tables in tables block and check
        for (default_amount_type i = 0; i < block.table_amount; i++)
        {
            table.Deserialize(block.data, block.tables_begin_address[i]);
            cout << "found table in .tvdbb: " << table.table_name << endl;
            if (table.table_name == table_name)
            {
                return true;
            }
        }

        // if not found, find in next block.
        while (block.next_block_pointer != 0)
        {
            // read next block
            ReadOneTableBlock(db_file.db_all_tables_path, block.next_block_pointer, block);   
            block.DeserializeFromBuffer(block.data);
            // deserialize the tables in tables block and check
            for (default_amount_type i = 0; i < block.table_amount; i++)
            {
                table.Deserialize(block.data, block.tables_begin_address[i]);
                if (table.table_name == table_name)
                {
                    return true;
                }
            }
        }
        
        // if still not found, return false
        return false;
    }

    /**
     * @brief Checks if a column exists in a table.
     *
     * @param ct The ColumnTable object that contains the table's metadata.
     * @param column_name The name of the column to check.
     * @param column A reference to a Columns object that will be populated with the column's metadata if it exists.
     *
     * @return True if the column exists, false otherwise.
     *
     * @example
     * ColumnTable table;
     * Columns column;
     * if (CheckColumnExist(table, "my_column", column)) {
     *     std::cout << "Column exists!" << std::endl;
     *     std::cout << "Column name: " << column.column_name_array[0] << std::endl;
     *     std::cout << "Column type: " << column.column_type_array[0] << std::endl;
     * } else {
     *     std::cout << "Column does not exist." << std::endl;
     * }
    */
    bool CheckColumnExist(ColumnTable ct, string column_name, Columns& column)
    {
        for (default_amount_type i = 0; i < ct.column_size; i++)
        {   
            if (ct.columns.column_name_array[i] == column_name)
            {   
                column.column_name_array = new string[0];
                column.column_name_array[0] = ct.columns.column_name_array[i];
                column.column_type_array = new default_enum_type[0];
                column.column_type_array[0] = ct.columns.column_type_array[i];
                column.column_length_array = new default_length_size[0];
                column.column_length_array[0] = ct.columns.column_length_array[i];
                column.column_index_type_array = new default_enum_type[0];
                column.column_index_type_array[0] = ct.columns.column_index_type_array[i];
                column.column_storage_address_array = new default_address_type[0];
                column.column_storage_address_array[0] = ct.columns.column_storage_address_array[i];
                return true;
            }
            
        }

        return false;
    }

public:
    DBManagement()
    {
        file_mm = new FileManagement();
        bfmm = new BlockFileManagement();
    }

    /**
     * @brief Cal and return the default table data path base on the base db path.
     * @return default table data path of the input db path.
     */
    string GetDefaultTableDataFolderOfBaseDbPath()
    {   
        string db_folder = GetBaseDbFolder();
        string table_path = GetDefaultTablePath(db_folder);

        // create table data folder
        string default_table_data_folder = table_path + "/" + DEFAULT_TABLE_DATA_FOLDER;
        file_mm->OpencvDirAndMkdir(default_table_data_folder);
        return default_table_data_folder;
    }
    
    /**
     * @brief Cal and return the default table data path base on the input db path.
     * @param table_path db folder path
     * @return default table data folder path of the input db path.
     */
    string GetAndCreateDataFolderPath(string db_path)
    {   
        string table_path = GetDefaultTablePath(db_path);
        string default_table_data_folder = table_path + "/" + DEFAULT_TABLE_DATA_FOLDER;
        file_mm->OpencvDirAndMkdir(default_table_data_folder);
        return default_table_data_folder;
    }

    /**
     * @brief Get the data file path of input table_name. if not exist, then create it.
     * @param data_folder_path data file store folder
     * @param table_name table name
     * @return data file path of input table_name
     */
    string GetAndCreateDataFile(string data_folder_path, string table_name)
    {
        string table_data_file = data_folder_path;
        table_data_file += "/";
        table_data_file += table_name;
        table_data_file += TABLE_DATA_FILE_SUFFIX;
        file_mm->ReadOrCreateFile(table_data_file);
        return table_data_file;
    }

    // store a db object to file
    void SerializeDBFile(DB& db, string file_path)
    {   
        char endl = '\n';
        file_mm->WriteFile(file_path, db.db_name.c_str(), db.db_name.length());
        file_mm->WriteFileAppend(file_path, &endl, 1);
        file_mm->WriteFileAppend(file_path, db.db_description.c_str(), db.db_description.length());
        file_mm->WriteFileAppend(file_path, &endl, 1);
        file_mm->WriteFileAppend(file_path, db.db_all_tables_path.c_str(), db.db_all_tables_path.length());
        file_mm->WriteFileAppend(file_path, &endl, 1);
    }
    
    // load a db object from filef
    void DeserializeDBFile(DB& db, string file_path)  
    {  
        std::ifstream file_read = file_mm->ReadOrCreateFile(file_path);
        getline(file_read, db.db_name);
        getline(file_read, db.db_description);
        getline(file_read, db.db_all_tables_path);
        file_read.close();
    } 

};

}

#endif // VDBMS_BENCH_DB_MANAGEMENT_H_