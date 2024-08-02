// Copyright (c) 2024 by dingning
//
// file  : condition_node.h
// since : 2024-07-26
// desc  : this is a Abstract Syntax Tree(AST) node, means here is a condition to check.

#ifndef VDBMS_SQL_ABSTRACT_SYNTAX_TREE_NODES_NODE_H_
#define VDBMS_SQL_ABSTRACT_SYNTAX_TREE_NODES_NODE_H_

#include <vector>
#include <string>

#include "../../../../meta/value.h"
#include "../../../../config.h"

using std::vector;
using std::string;

namespace tiny_v_dbms {

// means this base node is used to store which type of sql.
enum NodeType {
    CREATE_DATABASE_NODE,
    CREATE_TABLE_NODE
};

enum IndexType
{
    B_PLUS_TREE,
    UNIQUE,
    VECTOR_INDEX_1 // this type of index will not been finished in 0.0.1 version
};

struct DataBase
{
    string db_name;
};

struct Column
{
    string col_name;
    ValueType value_type;
    int col_length;
};

struct Index
{
    string index_name;
    string col_nname;
    IndexType index_type;
};

struct Table
{
    vector<Column> columns;
    vector<Index> indexs; 
};

// struct 


class BaseNode
{
private:
    NodeType node_type;
    
public:


};

}

#endif // VDBMS_SQL_ABSTRACT_SYNTAX_TREE_NODES_NODE_H_