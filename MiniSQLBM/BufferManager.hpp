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
    /**
     * Distribute a block from the pool when a new block file opened
     */
    int currentRBPPos;                              // Current recordBlockPool's position
    int currentIBPPos;                              // Current IndexBlockPool's position
    Block* recordBlockPool[RECORD_BLOCK_NUM];       // Record block pool
    IndexBlock* indexBlockPool[INDEX_BLOCK_NUM];    // Index block pool
    
    /**
     * If the block is opened, it can be found in the map
     * @key     file's name
     * @value   block's pointer
     */
    std::map<std::string, Block*> recordBlockMap;       // Record block map
    std::map<std::string, IndexBlock*> indexBlockMap;   // Index block map
    
    /* Add the file's suffix */
    std::string formatNotoString(int numNo);
    
    /* Get one block from the block pool */
    Block* getBlockFromRecordBlockPool();
    IndexBlock* getBlockFromIndexBlockPool();
    
    /* Close a block */
    void closeBlock(Block* block);
    void closeIndexBlock(IndexBlock* block);
public:
    BufferManager();
    ~BufferManager();
    
    /* Functions for Record Manager */
    bool createTable(std::string tableName);
    bool dropTable(std::string tableName);
    Block* getFirstBlock(std::string tableName);
    Block* getNextBlock(Block* blockNow, int mode);
    Block* getBlockByOffset(std::string tableName, int offset);
    
    /* Functions for Index Manager */
    IndexBlock* getIndexBlock(std::string tableName, std::string attr, int blockNo);
    IndexBlock* getIndexNewBlock(std::string tableName, std::string attr);
    bool deleteIndexBlock(std::string tableName, std::string attr, int blockNo);
};

#endif /* BufferManager_hpp */
