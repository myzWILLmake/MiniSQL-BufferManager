//
//  IndexBlock.hpp
//  MiniSQLBM
//
//  Created by Yunze on 15/10/14.
//  Copyright © 2015年 Yunze. All rights reserved.
//

#ifndef IndexBlock_hpp
#define IndexBlock_hpp

#include <stdio.h>

class IndexBlock {
public:
    char* address;
    bool pin;
    bool dirty;
    bool active;
    std::string tableName;
    std::string attrName;
    int blockNo;
    IndexBlock();
};

#endif /* IndexBlock_hpp */
