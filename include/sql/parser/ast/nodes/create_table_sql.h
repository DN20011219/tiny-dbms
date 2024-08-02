// Copyright (c) 2024 by dingning
//
// file  : condition_node.h
// since : 2024-07-26
// desc  : this is a Abstract Syntax Tree(AST) node, means here is a condition to check.

#ifndef VDBMS_SQL_ABSTRACT_SYNTAX_TREE_NODES_NODE_H_
#define VDBMS_SQL_ABSTRACT_SYNTAX_TREE_NODES_NODE_H_

#include <vector>

using std::vector;
using std::string;

namespace tiny_v_dbms {

class Node
{
private:
    Node* parent_node;
    vector<Node*> child_nodes;

public:


};

}

#endif // VDBMS_SQL_ABSTRACT_SYNTAX_TREE_NODES_NODE_H_