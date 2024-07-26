// Copyright (c) 2024 by dingning
//
// file  : separate_word.h
// since : 2024-07-25
// desc  : this is a word separating tools, use it can separate one sql to segments.

#ifndef VDBMS_SQL_SEPARATE_WORD_H_
#define VDBMS_SQL_SEPARATE_WORD_H_

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <mutex>

#include "../dictionary.h"

namespace tiny_v_dbms {

using std::string;

class SeparateWord
{

private:
    SeparateWord();
    ~SeparateWord();
    SeparateWord* instance;
    std::mutex singleton_lock;

    std::map<string, int> key_word_map;

public:
    SeparateWord* GetInstance();

    /**
     * @brief Separate SQL into segments, use " " as sepatate char
     * 
     * @param sql SQL string to be separated
     * @return std::vector<std::string> Vector of separated SQL segments
     */
    std::vector<std::string> SeparateFirst(const std::string& sql)
    {
        std::vector<std::string> segments;
        std::string currentSegment;

        for (char c : sql) {
            if (c == ' ' || c == '\t' || c == '\n' || c == '(' || c == ')' || c == ';'|| c == ',') {
                if (!currentSegment.empty()) {
                    segments.push_back(currentSegment);
                    currentSegment.clear();
                }
            } else {
                currentSegment += c;
            }
        }

        if (!currentSegment.empty()) {
            segments.push_back(currentSegment);
        }

        return segments;
    }
    

};

} // namespace tiny_v_dbms

#endif // VDBMS_SQL_SEPARATE_WORD_H_