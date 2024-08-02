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

    bool ParseSql(std::vector<Token> sql_tokens)
    {
        for(SqlPattern pattern : ALL_PATTERNS)
        {
            if (pattern.Match(sql_tokens))
            {
                return true;
            }
        }
        return false;
    }
    

private:

};

}
#endif // VDBMS_SQL_PARSER_PARSER_H_