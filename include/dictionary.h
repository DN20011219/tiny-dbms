// Copyright (c) 2024 by dingning
//
// file  : dictionary.h
// since : 2024-07-25
// desc  : this file defines all word need in sql, used to seperate sql.

#ifndef VDBMS_DICTIONARY_H_
#define VDBMS_DICTIONARY_H_

#include <string>
#include <vector>

namespace tiny_v_dbms {

    enum SqlKeyword {
        USE,
        SELECT,
        FROM,
        WHERE,
        AND,
        OR,
        NOT,
        IN,
        LIKE,
        BETWEEN,
        ORDER,
        BY,
        GROUP,
        HAVING,
        LIMIT,
        INSERT,
        INTO,
        VALUES,
        UPDATE,
        SET,
        DELETE,
        JOIN,
        LEFT,
        RIGHT,
        INNER,
        OUTER,
        FULL,
        CREATE,
        TABLE,
        DROP,
        ALTER,
        ADD,
        COLUMN,
        MODIFY,
        CHANGE,
        RENAME,
        TRUNCATE
    };

    const std::string SQL_KEY_WORDS[] = {
        "USE",
        "SELECT",
        "FROM",
        "WHERE",
        "AND",
        "OR",
        "NOT",
        "IN",
        "LIKE",
        "BETWEEN",
        "ORDER",
        "BY",
        "GROUP",
        "HAVING",
        "LIMIT",
        "INSERT",
        "INTO",
        "VALUES",
        "UPDATE",
        "SET",
        "DELETE",
        "JOIN",
        "LEFT",
        "RIGHT",
        "INNER",
        "OUTER",
        "FULL",
        "CREATE",
        "TABLE",
        "DROP",
        "ALTER",
        "ADD",
        "COLUMN",
        "MODIFY",
        "CHANGE",
        "RENAME",
        "TRUNCATE"
    };

    
}

#endif // VDBMS_DICTIONARY_H_