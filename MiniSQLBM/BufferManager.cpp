//
//  BufferManager.cpp
//  MiniSQLBM
//
//  Created by Yunze on 15/10/14.
//  Copyright © 2015年 Yunze. All rights reserved.
//

#include <fstream>

#include "BufferManager.hpp"

BufferManager::BufferManager() {
    // Initial the position for the pool
    currentIBPPos = 0;
    currentRBPPos = 0;
    // Initial recordBlockPool
    for (int i=0; i<RECORD_BLOCK_NUM; i++) {
        Block* nodeBlock = new Block();
        recordBlockPool[i] = nodeBlock;
    }
    // Initial indexBlockPool and initial memory space for indexBlock
    for (int i=0; i<INDEX_BLOCK_NUM; i++) {
        IndexBlock* nodeBlock = new IndexBlock();
        char* memptr = (char*) malloc(0x1000);
        nodeBlock->address = memptr;
        indexBlockPool[i] = nodeBlock;
    }
}

BufferManager::~BufferManager() {
    for (int i=0; i<RECORD_BLOCK_NUM; i++) {
        closeBlock(recordBlockPool[i]);
        delete recordBlockPool[i];
    }
    for (int i=0; i<INDEX_BLOCK_NUM; i++) {
        closeIndexBlock(indexBlockPool[i]);
        free(indexBlockPool[i]->address);
        delete indexBlockPool[i];
    }
}

std::string BufferManager::formatNotoString(int numNo) {
    char numChar0 = '0' + numNo / 10;
    char numChar1 = '0' + numNo % 10;
    std::string tmpString = "";
    tmpString += numChar0;
    tmpString += numChar1;
    return tmpString;
}

Block* BufferManager::getBlockFromRecordBlockPool() {
    Block* returnPtr = NULL;
    Block* currentBlock = recordBlockPool[currentRBPPos];
    if (currentBlock->active) {
        closeBlock(currentBlock);
    }
    currentBlock->active = true;
    returnPtr = currentBlock;
    currentRBPPos = (currentRBPPos + 1) % RECORD_BLOCK_NUM;
    return returnPtr;
}

void BufferManager::closeBlock(Block *block) {
    std::ofstream fout;
    std::string fileName = block->tableName + "_" + formatNotoString(block->blockNo);
    fout.open(fileName, std::ios::out | std::ios::binary);
    if (fout) {
        fout.write((char*)block->records, sizeof(block->records));
        fout.close();
    }
    // Set the block unactive
    block->active = false;
    // Erase the block from the map
    std::map<std::string, Block*>::iterator it = recordBlockMap.find(fileName);
    recordBlockMap.erase(it);
}

IndexBlock* BufferManager::getBlockFromIndexBlockPool() {
    IndexBlock* returnPtr = NULL;
    IndexBlock* currentBlock = indexBlockPool[currentIBPPos];
    while (currentBlock->active && currentBlock->pin) {
        currentIBPPos = (currentIBPPos + 1) % INDEX_BLOCK_NUM;
        currentBlock = indexBlockPool[currentIBPPos];
    }
    if (currentBlock->active) {
        closeIndexBlock(currentBlock);
    }
    currentBlock->active = true;
    returnPtr = currentBlock;
    currentIBPPos = (currentIBPPos + 1) % INDEX_BLOCK_NUM;
    return returnPtr;
}

void BufferManager::closeIndexBlock(IndexBlock *block) {
    std::ofstream fout;
    std::string fileName = block->tableName + "_" + block->attrName + "_" + formatNotoString(block->blockNo);
    fout.open(fileName, std::ios::out | std::ios::binary);
    if (fout) {
        fout.write(block->address, 0x1000);
        fout.close();
    }
    // Reset the indexBlock
    block->active = false;
    block->pin = false;
    block->dirty = false;
    block->tableName = "";
    block->attrName = "";
    block->blockNo = 0;
    // Erase the block from the map
    std::map<std::string, IndexBlock*>::iterator it = indexBlockMap.find(fileName);
    indexBlockMap.erase(it);
}

bool BufferManager::createTable(std::string tableName) {
    bool returnBool = false;
    std::ofstream fout;
    fout.open(tableName + "_" + formatNotoString(0), std::ios::out | std::ios::binary);
    if (fout) {
        Block* tmpBlock = new Block();
        for (int i=0; i < EACH_BLOCK_RECORDS; i++) {
            tmpBlock->records[i].empty = true;
        }
        fout.write((char*)tmpBlock->records, sizeof(tmpBlock->records));
        delete tmpBlock;
        returnBool = true;
        fout.close();
    }
    return returnBool;
}

bool BufferManager::dropTable(std::string tableName) {
    // Erase all the data from the map and pool
    for (int i=0; i<MAX_BLOCK_FILE_NUM; i++) {
        std::string tmpFileName = tableName + "_" + formatNotoString(i);
        std::map<std::string, Block*>::iterator it = recordBlockMap.find(tmpFileName);
        if (it == recordBlockMap.end())
            break;
        Block* tmpBlock = recordBlockMap[tmpFileName];
        tmpBlock->active = false;
        recordBlockMap.erase(it);
    }
    // Delete the files
    for (int i=0; i<MAX_BLOCK_FILE_NUM; i++) {
        std::string tmpFileName = tableName + "_" + formatNotoString(i);
        const char* tmpFileNameC = tmpFileName.c_str();
        int res = remove(tmpFileNameC);
        if (res)
            break;
    }
    return true;
}

Block* BufferManager::getFirstBlock(std::string tableName) {
    Block* returnPtr = NULL;
    std::string fileName = tableName + "_" + formatNotoString(0);
    if (recordBlockMap.find(fileName) != recordBlockMap.end()) {
        // If existed in the map
        returnPtr = recordBlockMap[fileName];
    } else {
        // To open the Block
        std::ifstream fin;
        fin.open(fileName, std::ios::in | std::ios::binary);
        if (fin) {
            Block* tmpBlock = getBlockFromRecordBlockPool();
            int tmpReCordNum = 0;
            fin.read((char*)tmpBlock->records, sizeof(tmpBlock->records));
            for (int i=0; i<EACH_BLOCK_RECORDS; i++) {
                if (!tmpBlock->records[i].empty)
                    tmpReCordNum++;
            }
            tmpBlock->recordNum = tmpReCordNum;
            tmpBlock->blockNo = 0;
            tmpBlock->tableName = tableName;
            recordBlockMap[fileName] = tmpBlock;
            returnPtr = tmpBlock;
            fin.close();
        } else {
            // Doesn't exist the first block file
        }
    }
    return returnPtr;
}

Block* BufferManager::getNextBlock(Block *blockNow) {
    Block* returnPtr = NULL;
    std::string nextBlockFileName = blockNow->tableName + "_" + formatNotoString(blockNow->blockNo + 1);
    if (recordBlockMap.find(nextBlockFileName) != recordBlockMap.end()) {
        // If existed in the map
        returnPtr = recordBlockMap[nextBlockFileName];
    } else {
        // To open the Block
        std::ifstream fin;
        fin.open(nextBlockFileName, std::ios::in | std::ios::binary);
        if (fin) {
            Block* tmpBlock = getBlockFromRecordBlockPool();
            int tmpRecordNum = 0;
            fin.read((char*)tmpBlock->records, sizeof(tmpBlock->records));
            for (int i=0; i<EACH_BLOCK_RECORDS; i++) {
                if (!tmpBlock->records[i].empty)
                    tmpRecordNum++;
            }
            tmpBlock->recordNum = tmpRecordNum;
            tmpBlock->tableName = blockNow->tableName;
            tmpBlock->blockNo = blockNow->blockNo + 1;
            recordBlockMap[nextBlockFileName] = tmpBlock;
            returnPtr = tmpBlock;
            fin.close();
        } else {
            // Dones't exist the next block file
            // Need to create the next block file
            std::ofstream fout;
            fout.open(nextBlockFileName, std::ios::out | std::ios::binary);
            if (fout) {
                Block* tmpBlock = getBlockFromRecordBlockPool();
                for (int i=0; i<EACH_BLOCK_RECORDS; i++) {
                    tmpBlock->records[i].empty = true;
                }
                fout.write((char*)tmpBlock->records, sizeof(tmpBlock->records));
                tmpBlock->recordNum = 0;
                tmpBlock->blockNo = blockNow->blockNo + 1;
                tmpBlock->tableName = blockNow->tableName;
                recordBlockMap[nextBlockFileName] = tmpBlock;
                returnPtr = tmpBlock;
                fout.close();
            }
        }
    }
    return returnPtr;
}

IndexBlock* BufferManager::getIndexBlock(std::string tableName, std::string attr, int blockNo) {
    IndexBlock* returnPtr = NULL;
    std::string fileName = tableName + "_" + attr + "_" + formatNotoString(blockNo);
    if (indexBlockMap.find(fileName) != indexBlockMap.end()) {
        // Existed in the map
        returnPtr = indexBlockMap[fileName];
    } else {
        std::ifstream fin;
        fin.open(fileName, std::ios::in | std::ios::binary);
        if (fin) {
            IndexBlock* tmpBlock = getBlockFromIndexBlockPool();
            fin.read(tmpBlock->address, 0x1000);
            tmpBlock->tableName = tableName;
            tmpBlock->attrName = attr;
            tmpBlock->blockNo = blockNo;
            tmpBlock->dirty = false;
            tmpBlock->pin = false;
            indexBlockMap[fileName] = tmpBlock;
            returnPtr = tmpBlock;
            fin.close();
        } else {
            // Error: The specified file doesn't exist
        }
    }
    return returnPtr;
}

IndexBlock* BufferManager::getIndexNewBlock(std::string tableName, std::string attr) {
    IndexBlock* returnPtr = NULL;
    for (int i=0; i<MAX_BLOCK_FILE_NUM; i++) {
        std::string tmpFileName = tableName + "_" + attr + "_" + formatNotoString(i);
        std::ifstream inFile(tmpFileName.c_str());
        // If the file doesn't exist
        if (!inFile.good()) {
            // Create the file
            std::ofstream fout;
            fout.open(tmpFileName, std::ios::out | std::ios::binary);
            fout.close();
            // Get the indexBlock
            IndexBlock* tmpBlock = getBlockFromIndexBlockPool();
            memset(tmpBlock->address, 0, 0x1000);
            tmpBlock->tableName = tableName;
            tmpBlock->attrName = attr;
            tmpBlock->blockNo = i;
            tmpBlock->dirty = false;
            tmpBlock->pin = false;
            indexBlockMap[tmpFileName] = tmpBlock;
            returnPtr = tmpBlock;
            
            break;
        }
    }
    return returnPtr;
}

bool BufferManager::deleteIndexBlock(std::string tableName, std::string attr, int blockNo) {
    std::string fileName = tableName + "_" + attr + "_" + formatNotoString(blockNo);
    std::map<std::string, IndexBlock*>::iterator it = indexBlockMap.find(fileName);
    if (it != indexBlockMap.end()) {
        IndexBlock* tmpBlock = indexBlockMap[fileName];
        memset(tmpBlock->address, 0, 0x1000);
        tmpBlock->pin = false;
        tmpBlock->dirty = false;
        tmpBlock->tableName = "";
        tmpBlock->attrName = "";
        tmpBlock->blockNo = 0;
        indexBlockMap.erase(it);
    } else {
        // The file haven't been opened
    }
    const char* fileNameC = fileName.c_str();
    remove(fileNameC);
    return true;
}

