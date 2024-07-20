// Copyright (c) 2024 by dingning
//
// file  : table_header.h
// since : 2024-07-20
// desc  : TODO.

#ifndef VDBMS_META_TABLE_TABLE_HEADER_H_
#define VDBMS_META_TABLE_TABLE_HEADER_H_

#include <string>



namespace tiny_v_dbms {

using std::string; 

class TableHeader
{

public:
    string table_name;
    string table_type;
};

}

#endif // VDBMS_META_TABLE_TABLE_HEADER_H_