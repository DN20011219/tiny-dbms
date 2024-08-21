// Copyright (c) 2024 by dingning
//
// file  : value_tag.h
// since : 2024-08-21
// desc  : TODO.

#ifndef VDBMS_META_VALUE_TAG_H_
#define VDBMS_META_VALUE_TAG_H_

#include "value.h"
#include "../config.h"

namespace tiny_v_dbms {

#ifndef VALUE_TAG
#define VALUE_TAG
    #define value_tag std::pair<default_long_int, Value>
    #define value_tag_pair default_long_int, Value
#endif // VALUE_TAG

}

#endif // VDBMS_META_VALUE_TAG_H_