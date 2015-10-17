//
//  Block.hpp
//  MiniSQLBM
//
//  Created by Yunze on 15/10/14.
//  Copyright © 2015年 Yunze. All rights reserved.
//

#ifndef Block_hpp
#define Block_hpp

#include <stdio.h>
#include <string>

#define RECORD_LEN          512
#define EACH_BLOCK_RECORDS  8

class Record {
public:
    bool empty;
    char data[RECORD_LEN - 1];
};


class Block {
public:
    std::string tableName;
    bool active;
    int blockNo;
    
    Record records[EACH_BLOCK_RECORDS];
    int recordNum;
    
    Block();
};

#endif /* Block_hpp */
