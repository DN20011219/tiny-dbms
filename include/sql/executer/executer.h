// Copyright (c) 2024 by dingning
//
// file  : executer.h
// since : 2024-08-06
// desc  : This executer will handle all sql request and return search result.

#ifndef VDBMS_SQL_EXECUTER_EXECUTER_H_
#define VDBMS_SQL_EXECUTER_EXECUTER_H_

#include "../parser/ast.h"
#include "operator.h"
#include "optimizer.h"

namespace tiny_v_dbms {

enum ExecutionState
{
    PREPARE,
    PLANNING,
    EXECUTING,
    WAITING,
    FINISH,
    ERROR
};


class SqlExecuter
{
private:
    Operator* op;
    Optimizer* opt;
    
public:

    SqlExecuter(Operator* op, Optimizer* opt) : op(op), opt(opt)
    {
        
    }


};

}

#endif // VDBMS_SQL_EXECUTER_EXECUTER_H_