// Copyright (c) 2024 by dingning
//
// file  : parser.h
// since : 2024-08-01
// desc  : this is a word parser tools, use it can analyze 
// whether the sql meets the syntax requirements.
// If the sql is right, then update the input AST for next step

#ifndef VDBMS_SQL_PARSER_PARSER_H_
#define VDBMS_SQL_PARSER_PARSER_H_

#include "ast.h"
#include "sql_pattern.h"
#include "word_separater.h"

namespace tiny_v_dbms {

class Parser
{

private:
    NodeBuilder* node_builder;

public:
    Parser()
    {
        node_builder = new NodeBuilder();
    }
    
    ~Parser()
    {
        delete node_builder;
    }

    NodeType ParseSql(std::vector<Token> sql_tokens)
    {
        int pattern_flag = 0;
        while (pattern_flag < ALL_PATTERNS.size())
        {
            if (ALL_PATTERNS[pattern_flag].Match(sql_tokens))
            {
                return ALL_PATTERNS_NODE_TYPE[pattern_flag];
            }
            pattern_flag++;
        }

        return UNSUPPORT_NODE;
    }

    AST* BuildAST(string sql_tokens)
    {
        std::vector<Token> tokens = Tokenize(sql_tokens);
        return node_builder->BuildNode(tokens, ParseSql(tokens));
    }

};

}
#endif // VDBMS_SQL_PARSER_PARSER_H_