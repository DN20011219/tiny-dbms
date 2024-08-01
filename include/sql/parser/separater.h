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
#include <cctype>

namespace tiny_v_dbms {

enum TokenType {
   KEYWORD,
   IDENTIFIER,
   OPERATOR,
   NUMBER,
   STRING,
   UNKNOWN,
   ERROR
};

struct Token {
    TokenType type;
    std::string value;
    std::string error_message;

    Token(TokenType type, std::string value, std::string error_message) : type(type), value(value), error_message(error_message){}
};

static const std::string keywords[] = {
    "SELECT", "FROM", "WHERE", "INSERT", "INTO", "VALUES",
    "UPDATE", "SET", "DELETE", "CREATE", "TABLE", "DROP",
    "ALTER", "ADD", "COLUMN", "AND", "OR", "NOT", "IN",
    "LIKE", "JOIN", "ON", "ORDER", "BY", "GROUP", "HAVING",
    "INT", "FLOAT", "VCHAR", "VECTOR"
};

bool isKeyword(const std::string& word) {

    for (const auto& keyword : keywords) {
        if (word == keyword) {
            return true;
        }
    }
    return false;
}

std::vector<Token> tokenize(const std::string& sql) 
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
            if (isKeyword(word)) {
                tokens.push_back(Token(KEYWORD, word, ""));
            } else {
                tokens.push_back(Token(IDENTIFIER, word, ""));
            }
        } else if (isdigit(sql[i])) {
            std::string number;
            while (i < sql.length() && isdigit(sql[i])) {
                number += sql[i];
                ++i;
            }
            tokens.push_back(Token(NUMBER, number, ""));
        } else if (sql[i] == '\'' || sql[i] == '\"') {
            char quoteType = sql[i];
            std::string str;
            str += sql[i++];
            while (i < sql.length() && sql[i] != quoteType) {
                str += sql[i++];
            }
            if (i < sql.length()) {
                str += sql[i++];
                tokens.push_back(Token(STRING, str, ""));
            } else {
                tokens.clear();
                return tokens;
            }
        } else if (ispunct(sql[i])) {
            std::string op;
            op += sql[i++];
            tokens.push_back(Token(OPERATOR, op, ""));
        } else {
            std::string invalid;
            invalid += sql[i++];
            tokens.clear();
            return tokens;
        }
    }

    return tokens;
}

void printTokens(const std::vector<Token>& tokens) {
   for (const auto& token : tokens) {
       std::string type;
       switch (token.type) {
           case KEYWORD: type = "KEYWORD"; break;
           case IDENTIFIER: type = "IDENTIFIER"; break;
           case OPERATOR: type = "OPERATOR"; break;
           case NUMBER: type = "NUMBER"; break;
           case STRING: type = "STRING"; break;
           case ERROR: type = "ERROR"; break;
           default: type = "UNKNOWN"; break;
       }
       std::cout << type << ": " << token.value;
       if (!token.error_message.empty()) {
           std::cout << " (" << token.error_message << ")";
       }
       std::cout << std::endl;
   }
}

}


#endif // VDBMS_SQL_PARSER_SEPARATER_H_