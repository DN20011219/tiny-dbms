// Copyright (c) 2024 by dingning
//
// file  : sql_pattern.h
// since : 2024-08-02
// desc  : this file will record all pattern, which contains all supported sql by this dbms.

#ifndef VDBMS_SQL_PARSER_AST_PATTERN_SQL_PATTERN_H_
#define VDBMS_SQL_PARSER_AST_PATTERN_SQL_PATTERN_H_

#include <vector>
#include <memory>
#include <utility>
#include <initializer_list>

#include "../../token.h"
#include <vector>

using std::vector;

namespace tiny_v_dbms {

class TokenPattern
{
    friend class SqlPattern;

    int tokens_num; 
    vector<Token> tokens_template;

    // if range_check == true, means: all tokens_template here 
    // will be used to check one input token, if anyone matched, return true
    bool range_check;
    
    // if next_patterns not empty, means here is more pattern must be matched behind this pattern. 
    vector<TokenPattern> next_patterns;
    
public: 

    TokenPattern() {};

    TokenPattern(const TokenPattern& obj)
    {
        tokens_num = obj.tokens_num;
        tokens_template = vector<Token>(obj.tokens_template);
        range_check = obj.range_check;
        next_patterns = obj.next_patterns;
    }

    TokenPattern(int tokens_num, std::initializer_list<Token> tokens, bool range_check = false, vector<TokenPattern> next_patterns = vector<TokenPattern>(0))
        : tokens_num(tokens_num), tokens_template(std::move(tokens)) 
    {
        this->range_check = range_check;
        this->next_patterns = next_patterns;
    }
    
    bool CertainCheck(vector<Token> input_tokens, int begin_check_offset)
    {   
        int template_check_offset = 0;
        while (template_check_offset < tokens_num)
        {
            // check every token's type and value, if not match the template, return false
            if (begin_check_offset + template_check_offset >= input_tokens.size())
                return false;
            switch (tokens_template[template_check_offset].type)
            {
                case KEYWORD_T: {    // to KEY_WORD, must check the KEY_WORD value, such as SELECT, CREATE
                    if (
                        input_tokens[begin_check_offset + template_check_offset].type != KEYWORD_T 
                        || 
                        input_tokens[begin_check_offset + template_check_offset].value != tokens_template[template_check_offset].value
                    ) 
                        return false;
                } break;
                case IDENTIFIER_T: {    // to ID, not need to further check
                    if (
                        input_tokens[begin_check_offset + template_check_offset].type != IDENTIFIER_T 
                    ) 
                        return false;
                } break;
                case OPERATOR_T: {    // to OPERATOR, must check the value, such as , ( )
                    if (
                        input_tokens[begin_check_offset + template_check_offset].type != OPERATOR_T 
                        || 
                        input_tokens[begin_check_offset + template_check_offset].value != tokens_template[template_check_offset].value
                    ) 
                        return false;
                } break;
                case NUMBER_T: {    // to VALUE token(NUMBER_T, STRING_T), maybe need more check, but now we only check if it's a VALUE
                    if (
                        input_tokens[begin_check_offset + template_check_offset].type != NUMBER_T 
                        && 
                        input_tokens[begin_check_offset + template_check_offset].type != STRING_T
                    )
                        return false;
                } break;
                case STRING_T: {    // as same as the NUMBER_T
                    if (
                        input_tokens[begin_check_offset + template_check_offset].type != NUMBER_T 
                        && 
                        input_tokens[begin_check_offset + template_check_offset].type != STRING_T
                    )
                        return false;
                } break;
                default:
                    return false;
            }
            template_check_offset++;
        }

        return true;
    }

    // mainly used to check support data type (types are defined as KEYWORD)
    bool RangeCheck(vector<Token> input_tokens, int begin_check_offset)
    {
        int template_check_offset = 0;
        while (template_check_offset < tokens_num)
        {
            if (begin_check_offset + template_check_offset >= input_tokens.size())
                return false;
            // check every token's type and value, if anyone match the template, return true
            switch (tokens_template[template_check_offset].type)
            {
                case KEYWORD_T: {    // to KEY_WORD, must check the KEY_WORD value, such as SELECT, CREATE
                    if (
                        input_tokens[begin_check_offset + template_check_offset].type == KEYWORD_T 
                        || 
                        input_tokens[begin_check_offset + template_check_offset].value == tokens_template[template_check_offset].value
                    ) 
                        return true;
                } break;
                case IDENTIFIER_T: {    // to ID, not need to further check
                    if (
                        input_tokens[begin_check_offset + template_check_offset].type == IDENTIFIER_T 
                    ) 
                        return true;
                } break;
                case OPERATOR_T: {    // to OPERATOR, must check the value, such as , ( )
                    if (
                        input_tokens[begin_check_offset + template_check_offset].type == OPERATOR_T 
                        && 
                        input_tokens[begin_check_offset + template_check_offset].value == tokens_template[template_check_offset].value
                    ) 
                        return true;
                } break;
                case NUMBER_T: {    // to VALUE token(NUMBER_T, STRING_T), maybe need more check, but now we only check if it's a VALUE
                    if (
                        input_tokens[begin_check_offset + template_check_offset].type == NUMBER_T 
                        && 
                        input_tokens[begin_check_offset + template_check_offset].type == STRING_T
                    )
                        return true;
                } break;
                case STRING_T: {    // as same as the NUMBER_T
                    if (
                        input_tokens[begin_check_offset + template_check_offset].type == NUMBER_T 
                        && 
                        input_tokens[begin_check_offset + template_check_offset].type == STRING_T
                    )
                        return true;
                } break;
                default:
                    return false;
            }
            template_check_offset++;
        }
        return false;
    }

    // first match this pattern, if there has more patern, recursively check.
    bool Match(vector<Token> input_tokens, int begin_check_offset)
    {
        // match the pattern behind.
        int next_pattern_offset = 0;
        int total_offset = tokens_num;
        while (next_pattern_offset < next_patterns.size())
        {
            if (!next_patterns[next_pattern_offset].Match(input_tokens, begin_check_offset + total_offset))
                return false;
            total_offset += next_patterns[next_pattern_offset].tokens_num;
            next_pattern_offset++;
        }

        // match this pattern
        if (range_check)
        {
            return RangeCheck(input_tokens, begin_check_offset);
        }
        return CertainCheck(input_tokens, begin_check_offset);
    }

    int GetMatchLength()
    {
        int this_pattern_length;
        if (range_check)
            this_pattern_length =  1;
        else 
            this_pattern_length = tokens_num;
        
        // cal the length of behind patterns.
        int next_pattern_offset = 0;
        while (next_pattern_offset < next_patterns.size())
        {
            this_pattern_length += next_patterns[next_pattern_offset].tokens_num;
            next_pattern_offset++;
        }

        return this_pattern_length;
    }
};

// all match pattern has been list here:

TokenPattern NEXT_ID(2, {Token(OPERATOR_T, ","), Token(IDENTIFIER_T, "")}); // , ID
TokenPattern NEXT_VALUE(2, {Token(OPERATOR_T, ","), Token(STRING_T, "")});  // , VALUE(string, int, string)

TokenPattern DATABASE(1, {Token(KEYWORD_T, "DATABASE")});

TokenPattern CREATE(1, {Token(KEYWORD_T, "CREATE")});
TokenPattern TABLE(1, {Token(KEYWORD_T, "TABLE")});
TokenPattern ID(1, {Token(IDENTIFIER_T, "")});
TokenPattern LEFT_BRACKET(1, {Token(OPERATOR_T, "(")});
TokenPattern VALUE_TYPE(4, {Token(KEYWORD_T, "INT"), Token(KEYWORD_T, "FLOAT"), Token(KEYWORD_T, "VCHAR"), Token(KEYWORD_T, "VECTOR")}, true);
vector<TokenPattern> NEXT_COLUMN_VALUE_TYPE({VALUE_TYPE});
TokenPattern NEXT_COLUMN(2, {Token(OPERATOR_T, ","), Token(IDENTIFIER_T, "")}, false, NEXT_COLUMN_VALUE_TYPE); // , ID DATA_TYPE
TokenPattern RIGHT_BRACKET(1, {Token(OPERATOR_T, ")")});
TokenPattern SEMICOLON(1, {Token(OPERATOR_T, ";")});

// CREATE DATABASE ID ;
vector<TokenPattern> CREATE_DATABASE_SQL_PATTERN({CREATE, DATABASE, ID, SEMICOLON});
vector<bool> CREATE_DATABASE_SQL_PATTERN_NEC({true, true, true, true});

// CREATE TABLE ID (ID VALUE_TYPE NEXT_COLUMN) ;
vector<TokenPattern> CREATE_TABLE_SQL_PATTERN({CREATE, TABLE, ID, LEFT_BRACKET, ID, VALUE_TYPE, NEXT_COLUMN, RIGHT_BRACKET, SEMICOLON});
vector<bool> CREATE_TABLE_SQL_PATTERN_NEC({true, true, true, true, true, true, false, true, true});

class SqlPattern
{
    vector<TokenPattern> paterns;   // all tokens construct a pattern
    vector<bool> necessary;  // whether a patern is must needed in this sql, such as NEXT_COLUMN, NEXT_VALUE, NEXT_ID

public:

    SqlPattern();
    ~SqlPattern(){}

    SqlPattern(vector<TokenPattern>& token_patterns, vector<bool>& necessary_vector)
    {
        if (token_patterns.size() != necessary_vector.size())
        {
            throw std::runtime_error("construct a fault sql pattern");
        }
        paterns = token_patterns;
        necessary = necessary_vector;
    }

    bool Match(vector<Token> tokens)
    {
        int check_pattern = 0;  // match pattern offset
        int check_offset = 0;   // token list begin match offset
        while (check_pattern < necessary.size())
        {
            // if this pattern must be matched
            if (necessary[check_pattern])
            {
                if (paterns[check_pattern].Match(tokens, check_offset))
                    check_offset += paterns[check_pattern].GetMatchLength();
                else
                    return false;
            }
            // if not, can match 0 - N times
            else
            {
                int match_times = 0;
                while(paterns[check_pattern].Match(tokens, check_offset))
                {
                    match_times++;
                    check_offset += match_times * paterns[check_pattern].GetMatchLength();
                } 
            }

            check_pattern++;
        }
        
        return true;
    }


};

SqlPattern CREATE_DATABASE(CREATE_DATABASE_SQL_PATTERN, CREATE_DATABASE_SQL_PATTERN_NEC);
SqlPattern CREATE_TABLE(CREATE_TABLE_SQL_PATTERN, CREATE_TABLE_SQL_PATTERN_NEC);


// store all pattern for using
vector<SqlPattern> ALL_PATTERNS({CREATE_DATABASE, CREATE_TABLE});



}

#endif // VDBMS_SQL_PARSER_AST_PATTERN_SQL_PATTERN_H_