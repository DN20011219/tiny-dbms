// Copyright (c) 2024 by dingning
//
// file  : condition_node.h
// since : 2024-07-26
// desc  : this is a Abstract Syntax Tree(AST) node, means here is a condition to check.

#ifndef VDBMS_SQL_ABSTRACT_SYNTAX_TREE_NODES_CONDITION_NODE_H_
#define VDBMS_SQL_ABSTRACT_SYNTAX_TREE_NODES_CONDITION_NODE_H_

#include <string>

#include "node.h"

using std::string;

namespace tiny_v_dbms {

// such as name = "alice"
class ConditionNode : public Node
{
private:
    Node* left_child;
    string condition_charactor;
    Node* right_child;
public:
    ConditionNode(Node* left, string condition, Node* right):left_child(left), condition_charactor(condition), right_child(right)
    {
        
    }


};

}

#endif // VDBMS_SQL_ABSTRACT_SYNTAX_TREE_NODES_CONDITION_NODE_H_