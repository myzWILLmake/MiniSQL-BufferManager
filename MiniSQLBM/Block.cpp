//
//  Block.cpp
//  MiniSQLBM
//
//  Created by Yunze on 15/10/14.
//  Copyright © 2015年 Yunze. All rights reserved.
//

#include "Block.hpp"

Block::Block() {
    this->tableName = "";
    this->active = false;
    this->blockNo = 0;
    this->recordNum = 0;
}