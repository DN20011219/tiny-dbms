// Copyright (c) 2024 by dingning
//
// file  : executer.h
// since : 2024-08-06
// desc  : This executer will handle all sql request and return search result.

#ifndef VDBMS_SQL_EXECUTER_EXECUTER_H_
#define VDBMS_SQL_EXECUTER_EXECUTER_H_

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

};

}

#endif // VDBMS_SQL_EXECUTER_EXECUTER_H_