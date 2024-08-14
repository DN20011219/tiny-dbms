// Copyright (c) 2024 by dingning
//
// file  : operator.h
// since : 2024-08-14
// desc  : TODO.

#ifndef VDBMS_SQL_EXECUTER_OPERATOR_H_
#define VDBMS_SQL_EXECUTER_OPERATOR_H_

namespace tiny_v_dbms {

class ColumnScan
{
    /**
     * @param table_name
     * @param column_name
     * @param criterion
     * @brief return record meet the criterion
     */
};

class ColumnComplete
{
    /**
     * @param one_column_data
     * @param to_compelete_column_names
     * @brief Concatenate data from different columns(one_column_data, to_compelete_columns) into a single result, other col's record need extra operate
     */
};

class ColumnInnerJoin
{
    /**
     * @param firsrt_column_data
     * @param second_column_data
     * 
     * @brief return records exists on both of columns
     */
};



}

#endif // VDBMS_SQL_EXECUTER_OPERATOR_H_