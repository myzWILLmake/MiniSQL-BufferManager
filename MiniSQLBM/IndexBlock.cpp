//
//  IndexBlock.cpp
//  MiniSQLBM
//
//  Created by Yunze on 15/10/14.
//  Copyright © 2015年 Yunze. All rights reserved.
//

#include "IndexBlock.hpp"

IndexBlock::IndexBlock() {
    this->address = NULL;
    this->pin = false;
    this->dirty = false;
    this->active = false;
    this->tableName = "";
    this->attrName = "";
    this->blockNo = 0;
}