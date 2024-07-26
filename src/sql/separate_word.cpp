// Copyright (c) 2024 by dingning
//
// file  : separate_word.h
// since : 2024-07-25
// desc  : this is a word separating tools, use it can separate one sql to segments.

#include "../../include/sql/separate_word.h"

using std::string;

namespace tiny_v_dbms {


    SeparateWord::SeparateWord() 
    {
        std::cout << "SingleInstance SeparateWord create success!" << std::endl;

        // init key_word_map
        for (int i = 0; i < SQL_KEY_WORDS->length(); i++)
        {
            key_word_map[SQL_KEY_WORDS[i]] = i;
        }
    };

    SeparateWord::~SeparateWord()
    {
        std::unique_lock<std::mutex> lock(singleton_lock);
        if (instance != nullptr)
        {
            delete instance;
            instance = nullptr;
        }
    }

    SeparateWord* SeparateWord::GetInstance()
    {
        if (instance == nullptr)
        {
            std::unique_lock<std::mutex> lock(singleton_lock);
            if (instance == nullptr)
            {
                instance = new (std::nothrow) SeparateWord();
            }
        }

        return instance;
    }


} // namespace tiny_v_dbms