// Copyright (c) 2024 by dingning
//
// file  : bd.h
// since : 2024-07-18
// desc  : TODO.

#ifndef VDBMS_META_DB_DB_H_
#define VDBMS_META_DB_DB_H_

#include <string>

using std::string;

namespace tiny_v_dbms {

struct DB
{
    string db_name;
    string db_description;
    string db_header_path; // the first table block of this db. so can find all tables from it. here use the url for first addressing.  
};


}

#endif // VDBMS_META_DB_DB_H_