// Copyright (c) 2024 by dingning
//
// file  : value_tag.h
// since : 2024-08-21
// desc  : tag and value, tag is a virtual id of value, one tag means one record.

#ifndef VDBMS_META_VALUE_TAG_H_
#define VDBMS_META_VALUE_TAG_H_

#include <vector>

#include "value.h"
#include "../config.h"

namespace tiny_v_dbms {

#ifndef VALUE_TAG
#define VALUE_TAG
    #define value_tag std::pair<default_long_int, Value>
    #define value_tag_pair default_long_int, Value
#endif // VALUE_TAG

struct Row
{
    default_length_size value_amount;
    size_t tag;
    std::vector<Value*> values;

    Row(size_t tag, Value* first_value, ...) : tag(tag)
    {
        va_list args;
        va_start(args, first_value);
        values.push_back(first_value);
        while (true) {
            Value* value = va_arg(args, Value*);
            if (value == nullptr) break;
            values.push_back(value);
        }
        va_end(args);
    }

    ~Row()
    {
        for (Value* item : values)
        {
            delete item;
        }
    }

    /**
     * Serializes the object into a string representation.
     * 
     * The resulting string will be in the format of "(value1, value2, ...)".
     * 
     * @return A string representation of the object.
     */
    string ToString()
    {
        string result = "("; // Initialize the result string with an opening parenthesis.

        for (const auto& item : values) // Iterate over the values in the object.
        {
            result += item->ToString() + ", "; // Append the serialized value to the result string.
        }

        result += ")"; // Close the parenthesis.

        return result; // Return the serialized string.
    }

    /**
     * Serializes the object into a string representation with a tag.
     * 
     * The resulting string will be in the format of "(tag: value1, value2, ...)".
     * 
     * @return A string representation of the object with a tag.
     */
    string ToStringWithTag()
    {
        string result = "("; // Initialize the result string with an opening parenthesis.

        result += std::to_string(tag) + ": "; // Append the tag to the result string.

        for (const auto& item : values) // Iterate over the values in the object.
        {
            result += item->ToString() + ", "; // Append the serialized value to the result string.
        }

        result += ")"; // Close the parenthesis.

        return result; // Return the serialized string with a tag.
    }
};

}

#endif // VDBMS_META_VALUE_TAG_H_