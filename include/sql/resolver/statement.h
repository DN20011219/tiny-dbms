// Copyright (c) 2024 by dingning
//
// file  : statement.h
// since : 2024-08-30
// desc  : .

#ifndef VDBMS_SQL_EXECUTER_STATEMENT_H_
#define VDBMS_SQL_EXECUTER_STATEMENT_H_

#include <string>
#include <vector>

#include "../sql_struct.h"

namespace tiny_v_dbms {

struct ProjectionStatement
{
    std::vector<std::string> col_names;

};

struct SelectionStatement
{
    CompareCondition* condition;
    
};

struct JoinStatement
{
    std::string left_records;
    std::string right_records;

    std::string left_join_col_name;
    std::string right_join_col_name;
};

struct AndStatement
{
    
};


}

#endif // VDBMS_SQL_EXECUTER_STATEMENT_H_