// Copyright (c) 2024 by dingning
//
// file  : optimizer.h
// since : 2024-08-06
// desc  : This file contains all optimizer for sql's excution plan. Mainly for select sql.

#ifndef VDBMS_SQL_EXECUTER_PLAN_H_
#define VDBMS_SQL_EXECUTER_PLAN_H_

#include "../parser/ast.h"
#include "operator.h"

namespace tiny_v_dbms {

class Optimizer
{
private:
    Operator* op;

public:

    SqlResponse* ExecuteSelect(SelectFromOneTableSql* sql)
    {
        
    }

};

}

#endif // VDBMS_SQL_EXECUTER_PLAN_H_