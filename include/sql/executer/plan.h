// Copyright (c) 2024 by dingning
//
// file  : plan.h
// since : 2024-08-06
// desc  : This file contains all type of sql's excution plan. 
// These plans will use the interface offered by below module (storage) to excute. 

#ifndef VDBMS_SQL_EXECUTER_PLAN_H_
#define VDBMS_SQL_EXECUTER_PLAN_H_

namespace tiny_v_dbms {

enum SqlExecutePlanType
{
    CREATE_DATABASE_PLAN,
    CREATE_TABLE_PLAN,
    INSERT_INTO_TABLE_PLAN,
    SELECT_FROM_SINGLE_TABLE_PLAN
};

class CreateDatabasePlan
{
    
};

class SqlExecutePlan
{

private:

    SqlExecutePlanType plan_type;
    

public:


};

}

#endif // VDBMS_SQL_EXECUTER_PLAN_H_