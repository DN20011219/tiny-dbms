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

#include "ast.h"
#include "token.h"
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
    
    // if next_patterns not empty, means here is more pattern must be matched behind this pattern. If range_check is true, anyone of next_patterns match will return true
    vector<TokenPattern> next_patterns;
    
    // this is a cache of match_length;
    int match_length;

public: 

    TokenPattern() {};

    TokenPattern(const TokenPattern& obj)
    {
        tokens_num = obj.tokens_num;
        tokens_template = vector<Token>(obj.tokens_template);
        range_check = obj.range_check;
        next_patterns = obj.next_patterns;
    }

    TokenPattern(int tokens_vector_size, std::initializer_list<Token> tokens, bool range_check = false, vector<TokenPattern> next_patterns = vector<TokenPattern>(0))
        : tokens_num(tokens_vector_size), tokens_template(std::move(tokens)) 
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
        // int template_check_offset = 0;
        // try match tokens_template
        for (Token token : tokens_template)
        {
            switch (token.type)
            {
                case KEYWORD_T: {    // to KEY_WORD, must check the KEY_WORD value, such as SELECT, CREATE
                    if (
                        input_tokens[begin_check_offset].type == KEYWORD_T 
                        || 
                        input_tokens[begin_check_offset].value == token.value
                        ) 
                    {
                        match_length = 1;
                        return true;
                    }
                } break;
                case IDENTIFIER_T: {    // to ID, not need to further check
                    if (
                        input_tokens[begin_check_offset].type == IDENTIFIER_T 
                        ) 
                    {
                        match_length = 1;
                        return true;
                    }
                } break;
                case OPERATOR_T: {    // to OPERATOR, must check the value, such as , ( )
                    if (
                        input_tokens[begin_check_offset].type == OPERATOR_T 
                        && 
                        input_tokens[begin_check_offset].value == token.value
                        ) 
                    {
                        match_length = 1;
                        return true;
                    }
                } break;
                case NUMBER_T: {    // to VALUE token(NUMBER_T, STRING_T), maybe need more check, but now we only check if it's a VALUE
                    if (
                        input_tokens[begin_check_offset].type == NUMBER_T 
                        || 
                        input_tokens[begin_check_offset].type == STRING_T
                        )
                    {
                        match_length = 1;
                        return true;
                    }
                } break;
                case STRING_T: {    // as same as the NUMBER_T
                    if (
                        input_tokens[begin_check_offset].type == NUMBER_T 
                        ||
                        input_tokens[begin_check_offset].type == STRING_T
                        )
                    {
                        match_length = 1;
                        return true;
                    }
                } break;
                default:
                    return false;
            }
        }

        // if next_patterns not empty, 
        // then try to match any one of next_patterns, if matched anyone of next, return true and set match_length as the matched pattern length.
        for (TokenPattern next_pattern: next_patterns)
        {
            if (next_pattern.Match(input_tokens, begin_check_offset))
            {
                match_length = next_pattern.GetMatchLength();
                return true;
            }
        }

        // not matched anyone pattern, set match_length as 0
        match_length = 0;
        return false;
    }

    // first match this pattern, if there has more patern, recursively check.
    bool Match(vector<Token> input_tokens, int begin_check_offset)
    {
        // range_check: one of this tokens_template and next_patterns matched will return true
        if (range_check)
        {
            return RangeCheck(input_tokens, begin_check_offset);
        }    


        // first match self
        if (!CertainCheck(input_tokens, begin_check_offset))
        {
            return false;
        }
        // match tokens_template
        int next_pattern_offset = 0;
        int total_offset = tokens_num;
        while (next_pattern_offset < next_patterns.size())
        {
            if (!next_patterns[next_pattern_offset].Match(input_tokens, begin_check_offset + total_offset))
            {
                return false;
            }    
            total_offset += next_patterns[next_pattern_offset].GetMatchLength();
            next_pattern_offset++;
        }

        return true;
    }

    int GetMatchLength()
    {
        if (range_check)
        {
            return match_length;
        } 
        else 
        {
            int length = tokens_num;
            // cal the length of behind patterns.
            int next_pattern_offset = 0;
            while (next_pattern_offset < next_patterns.size())
            {
                length += next_patterns[next_pattern_offset].GetMatchLength();
                next_pattern_offset++;
            }
            return length;
        }

    }
};

// all pattern can be matched has been list here:

TokenPattern ID(1, {Token(IDENTIFIER_T, "")});
TokenPattern NEXT_ID(2, {Token(OPERATOR_T, ","), Token(IDENTIFIER_T, "")}); // , ID

TokenPattern VALUE(1, {Token(STRING_T, "")});
TokenPattern NEXT_VALUE(2, {Token(OPERATOR_T, ","), Token(STRING_T, "")});  // , VALUE(string, int, string)


// BASE SQL
TokenPattern INSERT(1, {Token(KEYWORD_T, "INSERT")});
TokenPattern INTO(1, {Token(KEYWORD_T, "INTO")});
TokenPattern CREATE(1, {Token(KEYWORD_T, "CREATE")});
TokenPattern DATABASE(1, {Token(KEYWORD_T, "DATABASE")});
TokenPattern TABLE(1, {Token(KEYWORD_T, "TABLE")});
TokenPattern VALUES(1, {Token(KEYWORD_T, "VALUES")});
TokenPattern LEFT_BRACKET(1, {Token(OPERATOR_T, "(")});
TokenPattern VALUE_TYPE(4, {Token(KEYWORD_T, "INT"), Token(KEYWORD_T, "FLOAT"), Token(KEYWORD_T, "VCHAR"), Token(KEYWORD_T, "VECTOR")}, true);
vector<TokenPattern> NEXT_COLUMN_VALUE_V({VALUE_TYPE});
TokenPattern NEXT_COLUMN(2, {Token(OPERATOR_T, ","), Token(IDENTIFIER_T, "")}, false, NEXT_COLUMN_VALUE_V); // , ID DATA_TYPE
TokenPattern RIGHT_BRACKET(1, {Token(OPERATOR_T, ")")});
TokenPattern SEMICOLON(1, {Token(OPERATOR_T, ";")});

// SELECT SQL 
TokenPattern SELECT(1, {Token(KEYWORD_T, "SELECT")});
vector<TokenPattern> FIRST_COL_V({ID});
TokenPattern FIRST_COL(1, {Token(OPERATOR_T, "*")}, true, FIRST_COL_V); // * / ID
TokenPattern FROM(1, {Token(KEYWORD_T, "FROM")});
TokenPattern WHERE(1, {Token(KEYWORD_T, "WHERE")});

TokenPattern NOT_EQUAL_SIGN(2, {Token(OPERATOR_T, "!"), Token(OPERATOR_T, "=")});
vector<TokenPattern> NOT_EQUAL_V({NOT_EQUAL_SIGN}); // !=
TokenPattern COMPARER(3, {Token(OPERATOR_T, ">"), Token(OPERATOR_T, "<"), Token(OPERATOR_T, "=")}, true, NOT_EQUAL_V); // > < = !=
TokenPattern OPERATOR(2, {Token(KEYWORD_T, "AND"), Token(KEYWORD_T, "OR")}, true); // AND OR

vector<TokenPattern> CONDITION_V({WHERE, ID, COMPARER, VALUE}); // WHERE ID >/</=/!= VALUE
TokenPattern CONDITION(0, {}, false, CONDITION_V); // ID >/</=/!= VALUE
vector<TokenPattern> NEXT_CONDITION_V({OPERATOR, ID, COMPARER, VALUE}); // ADN/OR ID >/</=/!= VALUE
TokenPattern NEXT_CONDITION(0, {}, false, NEXT_CONDITION_V); // ADN/OR ID >/</=/!= VALUE

// belows are sql patterns:

// CREATE DATABASE ID ;
vector<TokenPattern> CREATE_DATABASE_SQL_PATTERN({CREATE, DATABASE, ID, SEMICOLON});
vector<bool> CREATE_DATABASE_SQL_PATTERN_NEC({true, true, true, true});

// CREATE TABLE ID (ID VALUE_TYPE NEXT_COLUMN) ;
vector<TokenPattern> CREATE_TABLE_SQL_PATTERN({CREATE, TABLE, ID, LEFT_BRACKET, ID, VALUE_TYPE, NEXT_COLUMN, RIGHT_BRACKET, SEMICOLON});
vector<bool> CREATE_TABLE_SQL_PATTERN_NEC({true, true, true, true, true, true, false, true, true});

// INSERT INTO ID (ID NEXT_ID) VALUES (VALUE, NEXT_VALUE) ;
vector<TokenPattern> INSERT_INTO_SQL_PATTERN({INSERT, INTO, ID, LEFT_BRACKET, ID, NEXT_ID, RIGHT_BRACKET, VALUES, LEFT_BRACKET, VALUE, NEXT_VALUE, RIGHT_BRACKET, SEMICOLON});
vector<bool> INSERT_INTO_SQL_PATTERN_NEC({true, true, true, true, true, false, true, true, true, true, false, true, true});

// SELECT ID NEXT_ID FROM ID WHERE ID >/</=/!= VALUE AND/OR ID >/</=/!= VALUE;
vector<TokenPattern>  SELECT_FROM_ONE_TABLE_SQL_PATTERN({SELECT, FIRST_COL, NEXT_ID, FROM, ID, CONDITION, NEXT_CONDITION, SEMICOLON});
vector<bool>  SELECT_FROM_ONE_TABLE_SQL_PATTERN_NEC({true, true, false, true, true, false, false, true});

// SELECT ID NEXT_ID FROM ID INNER JOIN ID ON ID = ID WHERE ID >/</=/!= VALUE


class SqlPatternMatcher
{
    vector<TokenPattern> paterns;   // all tokens construct a pattern
    vector<bool> necessary;  // whether a patern is must needed in this sql, such as NEXT_COLUMN, NEXT_VALUE, NEXT_ID

public:

    SqlPatternMatcher();
    ~SqlPatternMatcher(){}

    SqlPatternMatcher(vector<TokenPattern>& token_patterns, vector<bool>& necessary_vector)
    {
        if (token_patterns.size() != necessary_vector.size())
        {
            throw std::runtime_error("construct a fault sql pattern");
        }
        paterns = token_patterns;
        necessary = necessary_vector;
    }

    // match sql mode and create ast
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
                {
                    check_offset += paterns[check_pattern].GetMatchLength();
                }
                else
                {
                    return false;
                }    
            }
            // if not, can match 0 - N times
            else
            {
                while(paterns[check_pattern].Match(tokens, check_offset))
                {
                    check_offset += paterns[check_pattern].GetMatchLength();
                } 
            }

            check_pattern++;
        }
        
        return true;
    }


};


// belows are sql matcher tools:
SqlPatternMatcher CREATE_DATABASE(CREATE_DATABASE_SQL_PATTERN, CREATE_DATABASE_SQL_PATTERN_NEC);
SqlPatternMatcher CREATE_TABLE(CREATE_TABLE_SQL_PATTERN, CREATE_TABLE_SQL_PATTERN_NEC);
SqlPatternMatcher INSERT_INTO_TABLE(INSERT_INTO_SQL_PATTERN, INSERT_INTO_SQL_PATTERN_NEC);
SqlPatternMatcher SELECT_FROM_ONE_TABLE(SELECT_FROM_ONE_TABLE_SQL_PATTERN, SELECT_FROM_ONE_TABLE_SQL_PATTERN_NEC);

// store all match tool for using
vector<SqlPatternMatcher> ALL_PATTERNS(
    {
        CREATE_DATABASE, 
        CREATE_TABLE, 
        INSERT_INTO_TABLE,
        SELECT_FROM_ONE_TABLE
    }
);
vector<NodeType> ALL_PATTERNS_NODE_TYPE(
    {
        CREATE_DATABASE_NODE, 
        CREATE_TABLE_NODE, 
        INSERT_INTO_TABLE_NODE,
        SELECT_FROM_ONE_TABLE_NODE
    }
);

class NodeBuilder
{

public:
    AST* BuildNode(vector<Token> tokens, NodeType node_type)
    {
        AST* ast = nullptr;
        void* sql = nullptr;
        switch (node_type)
        {
        case CREATE_DATABASE_NODE:
            sql = new CreateDatabaseSql(tokens);
            ast = new AST(CREATE_DATABASE_NODE, sql);
            break;

        case CREATE_TABLE_NODE:
            sql = new CreateTableSql(tokens);
            ast = new AST(CREATE_TABLE_NODE, sql);
            break;

        case INSERT_INTO_TABLE_NODE:
            sql = new InsertIntoTableSql(tokens);
            ast = new AST(INSERT_INTO_TABLE_NODE, sql);
            break;
        case SELECT_FROM_ONE_TABLE_NODE:
            sql = new SelectFromOneTableSql(tokens);
            ast = new AST(SELECT_FROM_ONE_TABLE_NODE, sql);
            break;
        // TODO: more sql support is on designing

        case UNSUPPORT_NODE:
            return nullptr;
        default:
            return nullptr;
        }
        return ast;
    }
};

}

#endif // VDBMS_SQL_PARSER_AST_PATTERN_SQL_PATTERN_H_