// Copyright (c) 2024 by dingning
//
// file  : parser.h
// since : 2024-08-01
// desc  : this is a word parser tools, use it can analyze 
// whether the sql meets the syntax requirements.
// If the sql is right, then update the input AST for next step

#ifndef VDBMS_SQL_PARSER_PARSER_H_
#define VDBMS_SQL_PARSER_PARSER_H_

#include "ast/pattern/sql_pattern.h"

namespace tiny_v_dbms {

class Parser
{

public:

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
        throw std::runtime_error("Can not parse sql!");
    }

};

}
#endif // VDBMS_SQL_PARSER_PARSER_H_