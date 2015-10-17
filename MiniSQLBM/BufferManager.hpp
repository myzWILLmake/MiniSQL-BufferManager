//
//  BufferManager.hpp
//  MiniSQLBM
//
//  Created by Yunze on 15/10/14.
//  Copyright © 2015年 Yunze. All rights reserved.
//

#ifndef BufferManager_hpp
#define BufferManager_hpp

#include <stdio.h>
#include <map>

#include "Block.hpp"
#include "IndexBlock.hpp"

#define RECORD_BLOCK_NUM    1024
#define INDEX_BLOCK_NUM     1024
#define MAX_BLOCK_FILE_NUM  100

class BufferManager {
private:
    int currentRBPPos;  // Current recordBlockPool's position
    int currentIBPPos;  // Current IndexBlockPool's position
    Block* recordBlockPool[RECORD_BLOCK_NUM];
    IndexBlock* indexBlockPool[INDEX_BLOCK_NUM];
    
    std::map<std::string, Block*> recordBlockMap;
    std::map<std::string, IndexBlock*> indexBlockMap;
    
    std::string formatNotoString(int numNo);    // Files' suffix
    Block* getBlockFromRecordBlockPool();
    IndexBlock* getBlockFromIndexBlockPool();
    void closeBlock(Block* block);
    void closeIndexBlock(IndexBlock* block);
public:
    BufferManager();
    ~BufferManager();
    // Functions for Record Manager
    bool createTable(std::string tableName);
    bool dropTable(std::string tableName);
    Block* getFirstBlock(std::string tableName);
    Block* getNextBlock(Block* blockNow);
    // Functions for Index Manager
    IndexBlock* getIndexBlock(std::string tableName, std::string attr, int blockNo);
    IndexBlock* getIndexNewBlock(std::string tableName, std::string attr);
    bool deleteIndexBlock(std::string tableName, std::string attr, int blockNo);
};

#endif /* BufferManager_hpp */
