// Copyright (c) 2024 by dingning
//
// file  : value.h
// since : 2024-08-02
// desc  : this file contains all column type in this system. It will 
// supply the serialize, deserialize, compare, convert functions to upper.
// The serialize and deserialize will be decide by value type.
// If it's a number, string_value will be unusefull. If it's a vchar, 
// the num_value will store the char length.

#ifndef VDBMS_META_COLUMN_VALUE_H_
#define VDBMS_META_COLUMN_VALUE_H_

#include <string>

#include "../config.h"

using std::string;

namespace tiny_v_dbms {

enum ValueType {
    INT_T, 
    FLOAT_T,
    VCHAR_T
};

class Value
{

public:

    // this constructor is used when read or write from memory buffer (know it's type)
    Value(ValueType value_type) : value_type(value_type) {}

    // belows constructor are used when sql parsing
    Value(int int_value)
    {
        value_type = ValueType::INT_T;
        num_value.int_value = int_value;
    }

    Value(float float_value)
    {
        value_type = ValueType::FLOAT_T;
        num_value.float_value = float_value;
    }

    /**
     * Constructs a Value object from a given string.
     *
     * The numeric value of the Value object will be set to the length of the input string.
     * The string value will be moved from the input string to avoid unnecessary copies.
     *
     * @param string_value The input string to construct the Value object from.
     *
     * Example:
     * ```cpp
     * Value val("Hello, World!");
     * std::cout << val.num_value.int_value << std::endl;  // Outputs: 13
     * ```
     */
    Value(string string_value)
    {
        value_type = ValueType::VCHAR_T;
        num_value.int_value = string_value.size();
        string_value = std::move(string_value.c_str());
    }

    int GetIntValue()
    {
        switch (value_type)
        {
        case ValueType::INT_T: 
            return num_value.int_value;
        case ValueType::FLOAT_T: 
            return num_value.float_value;
        case ValueType::VCHAR_T: 
            // try conver string to int
            try {
                return std::stoi(string_value);
            } catch (std::exception& e) {
                throw std::runtime_error("can not parse string to int");
            }
        default:
            throw std::runtime_error("not support data type");
            break;
        }
    }

    float GetFloatValue()
    {
        switch (value_type)
        {
        case ValueType::INT_T: 
            return num_value.int_value;
        case ValueType::FLOAT_T: 
            return num_value.float_value;
        case ValueType::VCHAR_T: 
            // try conver string to int
            try {
                return std::stof(string_value);
            } catch (std::exception& e) {
                throw std::runtime_error("can not parse string to float");
            }
        default:
            throw std::runtime_error("not support data type");
            break;
        }
    }

    string GetStringValue()
    {
        switch (value_type)
        {
        case ValueType::INT_T: 
            return std::to_string(num_value.int_value);
        case ValueType::FLOAT_T: 
            return std::to_string(num_value.float_value);
        case ValueType::VCHAR_T: 
            return string_value;
        default:
            throw std::runtime_error("not support data type");
            break;
        }
    }

    /**
     * Serialize the data into a buffer
     * 
     * @param data Buffer pointer
     * @param offset Buffer offset
     */
    void Serialize(char* data, default_address_type offset)
    {
        if (value_type == ValueType::VCHAR_T)
        {
            memcpy(data + offset, &num_value, sizeof(num_value));
            offset += sizeof(num_value);

            memcpy(data + offset, string_value, num_value.int_value);
            
            return;
        }
        else
        {
            memcpy(data + offset, &num_value, sizeof(num_value));
        }
    }

    /**
     * Deserialize the data from a buffer
     * 
     * @param data Buffer pointer
     * @param offset Buffer offset
     */
    void Deserialize(char* data, default_address_type offset)
    {
        memcpy(&num_value, data + offset, sizeof(num_value));
        offset += sizeof(num_value);

        if (value_type == ValueType::VCHAR_T)
        {
            string_value = new char(num_value.int_value);
            memcpy(string_value, data + offset, num_value.int_value);
        }
    }

private:
    ValueType value_type;
    union {
        int int_value;
        float float_value;
    } num_value;
    char* string_value;

    friend class ValueComparer;
};


class ValueComparer
{
    
public:
    static int Compare(Value a, Value b)
    {
        if (a.value_type == b.value_type)
        {
            switch (a.value_type)
            {
            case ValueType::INT_T:
                return a.num_value.int_value - b.num_value.int_value;
            case ValueType::FLOAT_T:
                return (a.num_value.float_value > b.num_value.float_value) - (a.num_value.float_value < b.num_value.float_value);
            case ValueType::VCHAR_T:
                return strcmp(a.string_value, b.string_value);
            default:
                throw std::runtime_error("not support data type");
            }
        }
        else if ((a.value_type == ValueType::INT_T && b.value_type == ValueType::FLOAT_T) ||
                 (a.value_type == ValueType::FLOAT_T && b.value_type == ValueType::INT_T))
        {
            float aValue = (a.value_type == ValueType::INT_T) ? a.num_value.int_value : a.num_value.float_value;
            float bValue = (b.value_type == ValueType::INT_T) ? b.num_value.int_value : b.num_value.float_value;
            return (aValue > bValue) - (aValue < bValue);
        }
        else if ((a.value_type == ValueType::INT_T || a.value_type == ValueType::FLOAT_T) && b.value_type == ValueType::VCHAR_T)
        {
            float bValue = std::stof(b.string_value);
            float aValue = (a.value_type == ValueType::INT_T) ? a.num_value.int_value : a.num_value.float_value;
            return (aValue > bValue) - (aValue < bValue);
        }
        else if (a.value_type == ValueType::VCHAR_T && (b.value_type == ValueType::INT_T || b.value_type == ValueType::FLOAT_T))
        {
            float aValue = std::stof(a.string_value);
            float bValue = (b.value_type == ValueType::INT_T) ? b.num_value.int_value : b.num_value.float_value;
            return (aValue > bValue) - (aValue < bValue);
        }
        else
        {
            throw std::runtime_error("not support data type");
        }
    }
};

}
#endif // VDBMS_META_COLUMN_VALUE_H_