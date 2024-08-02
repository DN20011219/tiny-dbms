// Copyright (c) 2024 by dingning
//
// file  : separater.h
// since : 2024-07-25
// desc  : this is a word separating tools, use it can separate one sql to segments and store the segment type.

#ifndef VDBMS_SQL_PARSER_SEPARATER_H_
#define VDBMS_SQL_PARSER_SEPARATER_H_

#include <iostream>
#include <string>
#include <vector>

#include "token.h"
#include "ast/pattern/sql_pattern.h"

namespace tiny_v_dbms {

bool IsKeyword(const std::string& word) {

    for (const auto& keyword : KEY_WORDS) {
        if (word == keyword) {
            return true;
        }
    }
    return false;
}

std::vector<Token> Tokenize(const std::string& sql) 
{
   std::vector<Token> tokens;
   size_t i = 0;

   while (i < sql.length()) 
   {
        if (isspace(sql[i])) 
        {
            ++i;
           continue;
        }

        if (isalpha(sql[i]) || sql[i] == '_') {
            std::string word;
            while (i < sql.length() && (isalnum(sql[i]) || sql[i] == '_')) {
                word += toupper(sql[i]);
                ++i;
            }
            if (IsKeyword(word)) {
                tokens.push_back(Token(KEYWORD_T, word));
            } else {
                tokens.push_back(Token(IDENTIFIER_T, word));
            }
        } else if (isdigit(sql[i])) {
            std::string number;
            while (i < sql.length() && isdigit(sql[i])) {
                number += sql[i];
                ++i;
            }
            tokens.push_back(Token(NUMBER_T, number));
        } else if (sql[i] == '\'' || sql[i] == '\"') {
            char quoteType = sql[i];
            std::string str;
            str += sql[i++];
            while (i < sql.length() && sql[i] != quoteType) {
                str += sql[i++];
            }
            if (i < sql.length()) {
                str += sql[i++];
                tokens.push_back(Token(STRING_T, str));
            } else {
                tokens.clear();
                return tokens;
            }
        } else if (ispunct(sql[i])) {
            std::string op;
            op += sql[i++];
            tokens.push_back(Token(OPERATOR_T, op));
        } else {
            tokens.clear();
            return tokens;
        }
    }

    return tokens;
}

void PrintTokens(const std::vector<Token>& tokens) {
   for (Token token : tokens) {
       std::string type;
       switch (token.type) {
           case KEYWORD_T: type = "KEYWORD"; break;
           case IDENTIFIER_T: type = "IDENTIFIER"; break;
           case OPERATOR_T: type = "OPERATOR"; break;
           case NUMBER_T: type = "NUMBER"; break;
           case STRING_T: type = "STRING"; break;
           case ERROR_T: type = "ERROR"; break;
           default: type = "UNKNOWN"; break;
       }
       std::cout << type << ": " << token.value;
       std::cout << std::endl;
   }
}

}


#endif // VDBMS_SQL_PARSER_SEPARATER_H_