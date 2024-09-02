// Copyright (c) 2024 by dingning
//
// file  : value_tag.h
// since : 2024-08-21
// desc  : tag and value, tag is a virtual id of value, one tag means one record.

#ifndef VDBMS_META_VALUE_TAG_H_
#define VDBMS_META_VALUE_TAG_H_

#include <vector>
#include <initializer_list>

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

    Row(size_t tag, std::initializer_list<Value*> init_list) : tag(tag), values(init_list) {
    
    }

    // ~Row()
    // {
    //     for (Value*& item : values)
    //     {
    //         if (item != nullptr)
    //         {
    //             delete item;
    //             item = nullptr;
    //         }
    //     }
    // }

    /**
     * Serializes the object into a string representation.
     * 
     * The resulting string will be in the format of "(value1, value2, ...)".
     * 
     * @return A string representation of the object.
     */
    string ToString()
    {
        string result = "| "; // Initialize the result string with an opening parenthesis.

        for (const auto& item : values) // Iterate over the values in the object.
        {
            result += item->ToString() + " | "; // Append the serialized value to the result string.
        }
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

        string result = std::to_string(tag) + ": "; // Append the tag to the result string.
        
        result += "| "; // Initialize the result string with an opening parenthesis.

        for (const auto& item : values) // Iterate over the values in the object.
        {
            result += item->ToString() + " | "; // Append the serialized value to the result string.
        }

        return result; // Return the serialized string with a tag.
    }

    /**
     * Converts the object to a string representation, 
     * including the specified number of values.
     * 
     * @param selected_amount The number of values to include in the string.
     * @return A string representation of the object.
     */
    string ToString(default_amount_type selected_amount)
    {
        string result = "| "; // Initialize the result string with an opening parenthesis.

        for (default_amount_type item = 0; item < values.size() && item < selected_amount; item++) // Iterate over the values in the object.
        {
            result += values[item]->ToString() + " | "; // Append the serialized value to the result string.
        }
        return result; // Return the serialized string.
    }
    
    /**
     * Converts the object to a string representation, 
     * including the specified number of values and the object's tag.
     * 
     * @param selected_amount The number of values to include in the string.
     * @return A string representation of the object, including its tag.
     */
    string ToStringWithTag(default_amount_type selected_amount)
    {

        string result = std::to_string(tag) + ": "; // Append the tag to the result string.
        
        result += "| "; // Initialize the result string with an opening parenthesis.

        for (default_amount_type item = 0; item < values.size() && item < selected_amount; item++) // Iterate over the values in the object.
        {
            result += values[item]->ToString() + " | "; // Append the serialized value to the result string.
        }

        return result; // Return the serialized string with a tag.
    }
};

}

#endif // VDBMS_META_VALUE_TAG_H_