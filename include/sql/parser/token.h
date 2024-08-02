// Copyright (c) 2024 by dingning
//
// file  : TODO
// since : 2024-07-TODO
// desc  : TODO.

#ifndef VDBMS_SQL_PARSER_TOKEN_H_
#define VDBMS_SQL_PARSER_TOKEN_H_

#include <string>
#include <vector>

namespace tiny_v_dbms {

enum TokenType {
   KEYWORD_T,
   IDENTIFIER_T,
   OPERATOR_T,
   NUMBER_T,
   STRING_T,
   UNKNOWN_T,
   ERROR_T
};

struct Token {
    TokenType type;
    std::string value;

    Token(TokenType type, std::string value) : type(type), value(value) {}
};

static const std::string KEY_WORDS[] = {
    "SELECT", "FROM", "WHERE", "INSERT", "INTO", "VALUES",
    "UPDATE", "SET", "DELETE", "CREATE", "TABLE", "DROP",
    "ALTER", "ADD", "COLUMN", "AND", "OR", "NOT", "IN",
    "LIKE", "JOIN", "ON", "ORDER", "BY", "GROUP", "HAVING",
    "INT", "FLOAT", "VCHAR", "VECTOR",  // support data type
    "DATABASE"
};

}

#endif // VDBMS_SQL_PARSER_TOKEN_H_