// Copyright (c) 2024 by dingning
//
// file  : value.h
// since : 2024-08-02
// desc  : this file contains all column type in this system. It will 
// supply the serialize, deserialize, compare, convert functions to upper.
// The serialize and deserialize will be decide by value type.
// If it's a number, string_value will be unusefull. If it's a vchar, 
// the num_value will store the char length.

#ifndef VDBMS_META_VALUE_H_
#define VDBMS_META_VALUE_H_

#include <string>

#include "../config.h"

using std::string;

namespace tiny_v_dbms {

enum ValueType {
    INT_T, 
    FLOAT_T,
    VCHAR_T,

    RAW_VALUE  // used when value is read from sql and not get it's type from stored table file
};

class Value
{

public:

    string raw_value;

    // copy constructor
    Value(const Value& other) : value_type(other.value_type) {
        if (other.string_value != nullptr) {
            string_value = new char[other.num_value.int_value];
            memcpy(string_value, other.string_value, other.num_value.int_value);
        } else {
            string_value = nullptr;
        }

        num_value = other.num_value;
        raw_value = other.raw_value;
    }

    Value(Value&& other) : value_type(other.value_type) {
        string_value = other.string_value;
        other.string_value = nullptr;

        num_value = other.num_value;
        raw_value = std::move(other.raw_value);
    }
    
    Value& operator=(const Value& other) {
        if (this != &other) {
            if (string_value != nullptr) {
                delete[] string_value;
            }

            value_type = other.value_type;

            if (other.string_value != nullptr) {
                string_value = new char[other.num_value.int_value];
                memcpy(string_value, other.string_value, other.num_value.int_value);
            } else {
                string_value = nullptr;
            }

            num_value = other.num_value;
            raw_value = other.raw_value;
        }

        return *this;
    }
    
    ~Value()
    {
        if (string_value != nullptr)
        {
            delete[] string_value;
        }
    }

    // onlu init value_type
    Value(ValueType value_type) : value_type(value_type) {
        string_value = nullptr;
    }

    // this constructor is used when not know it's type but has value.
    // So this time can use raw_value to store data for future value check and convert.
    Value (string value){
        value_type = ValueType::RAW_VALUE;
        raw_value = value;

        string_value = nullptr;
    }

    Value(int int_value)
    {
        value_type = ValueType::INT_T;
        num_value.int_value = int_value;

        string_value = nullptr;
    }

    Value(float float_value)
    {
        value_type = ValueType::FLOAT_T;
        num_value.float_value = float_value;

        string_value = nullptr;
    }

    Value(char* value, int value_length)
    {
        value_type = ValueType::VCHAR_T;
        num_value.int_value = value_length;
        string_value = new char[value_length];
        memcpy(string_value, value, value_length);
    }

    // this function is used after set raw value, and know the type of value
    bool InitValue(ValueType type)
    {
        switch (type)
        {
        case INT_T:
            try {
                num_value.int_value = std::stoi(raw_value);
                value_type = INT_T;
            } catch (std::exception& e) {
                return false;
                // throw std::runtime_error("can not parse string to int");
            }
            break;
        case FLOAT_T:
            try {
                num_value.float_value = std::stof(raw_value);
                value_type = FLOAT_T;
            } catch (std::exception& e) {
                return false;
                // throw std::runtime_error("can not parse string to float");
            }
            break;
        case VCHAR_T:
            num_value.int_value = raw_value.size();
            string_value = new char[raw_value.size()];
            memcpy(string_value, raw_value.c_str(), raw_value.size());
            value_type = VCHAR_T;
            break;
        default:
            return false;
            // throw std::runtime_error("Value: " + raw_value + " 's type is raw, need to be initialized!");
        }
        return true;
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
        else if (value_type == ValueType::RAW_VALUE)
        {
            throw std::runtime_error("Raw value can not be Serialize to buffer");
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


    string ToString() const
    {
        switch (value_type) {
            case INT_T:
                return std::to_string(num_value.int_value);
            case FLOAT_T:
                return std::to_string(num_value.float_value);
            case VCHAR_T:
                return string(string_value, num_value.int_value);
            case RAW_VALUE:
                return raw_value;
            default:
                throw std::runtime_error("Unsupported value type");
        }
    }

    default_length_size GetValueLength()
    {
        if (value_type == ValueType::VCHAR_T)
        {
            return sizeof(num_value) + num_value.int_value;
        }
        else if (value_type == ValueType::RAW_VALUE)
        {
            return raw_value.length();
        }
        else
        {
            return sizeof(num_value);
        }
    }

public:
    ValueType value_type;
    union {
        int int_value;
        float float_value;
    } num_value;
    char* string_value;
};

/**
 * Compares two values of different types.
 * 
 * This function compares two values of different types, including integers, floats, and strings.
 * It returns an integer value indicating the result of the comparison:
 *   - a negative value if a is less than b
 *   - zero if a is equal to b
 *   - a positive value if a is greater than b
 * 
 * @param a The first value to compare.
 * @param b The second value to compare.
 * @return An integer value indicating the result of the comparison.
 */
int Compare(Value* a, Value* b)
{
    if (a->value_type == b->value_type)
    {
        switch (a->value_type)
        {
            case ValueType::INT_T:
                return a->num_value.int_value - b->num_value.int_value;
            case ValueType::FLOAT_T:
                return (a->num_value.float_value > b->num_value.float_value) - (a->num_value.float_value < b->num_value.float_value);
            case ValueType::VCHAR_T:
                return strcmp(a->string_value, b->string_value);
            default:
                throw std::runtime_error("not support data type");
        }
    }
    else if ((a->value_type == ValueType::INT_T && b->value_type == ValueType::FLOAT_T) ||
        (a->value_type == ValueType::FLOAT_T && b->value_type == ValueType::INT_T))
    {
        float aValue = (a->value_type == ValueType::INT_T) ? a->num_value.int_value : a->num_value.float_value;
        float bValue = (b->value_type == ValueType::INT_T) ? b->num_value.int_value : b->num_value.float_value;
        return (aValue > bValue) - (aValue < bValue);
    }
    else if ((a->value_type == ValueType::INT_T || a->value_type == ValueType::FLOAT_T) && b->value_type == ValueType::VCHAR_T)
    {
        float bValue = std::stof(b->string_value);
        float aValue = (a->value_type == ValueType::INT_T) ? a->num_value.int_value : a->num_value.float_value;
        return (aValue > bValue) - (aValue < bValue);
    }
    else if (a->value_type == ValueType::VCHAR_T && (b->value_type == ValueType::INT_T || b->value_type == ValueType::FLOAT_T))
    {
        float aValue = std::stof(a->string_value);
        float bValue = (b->value_type == ValueType::INT_T) ? b->num_value.int_value : b->num_value.float_value;
        return (aValue > bValue) - (aValue < bValue);
    }
    else
    {
        throw std::runtime_error("not support data type");
    }
}

ValueType GetValueTypeFromStr(string value_str)
{
    if (value_str == "INT")
    {
        return INT_T;
    }
    if (value_str == "FLOAT")
    {
        return FLOAT_T;
    }
        if (value_str == "VCHAR")
    {
        return VCHAR_T;
    }

    throw std::runtime_error("can not parse value type: " + value_str);
}

ValueType GetType(default_enum_type type)
{
    return ValueType(type);
}

default_length_size GetValueTypeLength(ValueType type)
{
    switch (type)
    {
        case INT_T:
            return INT_LENGTH;
        case FLOAT_T:
            return FLOAT_LENGTH;
        case VCHAR_T:
            return VCHAR_LENGTH;
        case RAW_VALUE:
            return RAW_LENGTH;
        default:
            throw std::runtime_error("Can not get value length");
    }
}

Value* SerializeValueFromBuffer(ValueType type, char* buffer, default_address_type& offset)
{
    Value* value;

    switch (type)
    {
        case INT_T:
        {
            int val_int;
            memcpy(&val_int, buffer + offset, INT_LENGTH);
            value = new Value(val_int);
            break;
        }
        case FLOAT_T:
        {
            float val_float;
            memcpy(&val_float, buffer + offset, FLOAT_LENGTH);
            value = new Value(val_float);
            break;
        }
        case VCHAR_T:
        {
            int str_length;
            memcpy(&str_length, buffer + offset, sizeof(int));
            char* val_c = new char[str_length];
            memcpy(val_c, buffer + offset + sizeof(int), str_length);
            value = new Value(val_c, str_length);
            delete[] val_c;
            break;
        }
        case RAW_VALUE:
        {
            char* raw_val_c = new char[RAW_LENGTH];
            memcpy(raw_val_c, buffer + offset, RAW_LENGTH);
            string raw_val_str(raw_val_c);
            value = new Value(raw_val_str);
            delete[] raw_val_c;
            break;
        }
    }

    return value;
}

Value* BuildDefaultValue(ValueType value_type)
{
    Value* value = nullptr;
    switch (value_type)
    {
        case INT_T:
        {
            int int_val = 0;
            value = new Value(int_val);
            break;
        }
        case FLOAT_T:
        {
            float float_val = 0;
            value = new Value(float_val);
            break;
        }
        case VCHAR_T:
        {
            int char_val_len = 1;
            char* char_val = new char[1];
            char_val[0] = ' ';
            value = new Value(char_val, char_val_len);
            break;
        }
        default:
            throw std::runtime_error("Can not get value length");
    }

    return value;
}

}
#endif // VDBMS_META_VALUE_H_