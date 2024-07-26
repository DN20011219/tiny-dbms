// Copyright (c) 2024 by dingning
//
// file  : log_block.h
// since : 2024-07-26
// desc  : this type block is used to store log. Log is a type of data, 
// it's used to store the change of tuples, such as update, delete, so 
// that the data change cost could be less.

/*

one row of log is designed like this: | |

| address |  data description |

0 ---------block begin 4kb----------------------
0 | field length (20) |
4 | field_data_nums (1)|
8 | pre_block_pointer (0x0000) |
12| next_block_pointer (0x0000) |

4075 | field data (contains 20 byte data)|
4096 ------block end----------------------------

*/

#ifndef VDBMS_META_BLOCK_LOG_BLOCK_H_
#define VDBMS_META_BLOCK_LOG_BLOCK_H_

namespace tiny_v_dbms {

class LogBlock
{

private:
    

public:


};

}

#endif // VDBMS_META_BLOCK_LOG_BLOCK_H_