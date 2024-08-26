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

    /**
     * Serializes the object into a string representation.
     * 
     * The resulting string will be in the format of "(value1, value2, ...)".
     * 
     * @return A string representation of the object.
     */
    string Serialize()
    {
        string result = "("; // Initialize the result string with an opening parenthesis.

        for (const auto& item : values) // Iterate over the values in the object.
        {
            char* val; // Declare a pointer to a character array to store the serialized value.
            item->Serialize(val, 0); // Serialize the value into the character array.
            string val_str(val); // Create a string from the character array.
            result += val_str + ", "; // Append the serialized value to the result string.
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
    string TagsSerialize()
    {
        string result = "("; // Initialize the result string with an opening parenthesis.

        result += std::to_string(tag) + ": "; // Append the tag to the result string.

        for (const auto& item : values) // Iterate over the values in the object.
        {
            char* val; // Declare a pointer to a character array to store the serialized value.
            item->Serialize(val, 0); // Serialize the value into the character array.
            string val_str(val); // Create a string from the character array.
            result += val_str + ", "; // Append the serialized value to the result string.
        }

        result += ")"; // Close the parenthesis.

        return result; // Return the serialized string with a tag.
    }
};

}

#endif // VDBMS_META_VALUE_TAG_H_