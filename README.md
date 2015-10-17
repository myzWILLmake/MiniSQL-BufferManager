# MiniSQL-BufferManager
A part of co-operative work for DSD(Database System Design)

## Issues

1.	`DropTable`函数中，因为要求在table上的所有index也需要全部删除。这里 RecordManager 和 IndexManager 协商下看怎么解决比较好
2. `Block`头文件和`IndexBlock`头文件有所更新，查看下改动吧。

Block:

```cpp
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
    bool active;				//代表该block使用中
    int blockNo;
    
    Record records[EACH_BLOCK_RECORDS];
    int recordNum;
    
    Block();
};
```

IndexBlock:

```cpp
class IndexBlock {
public:
    char* address;
    bool pin;
    bool dirty;
    bool active;					//代表该block使用中
    std::string tableName;		//加上table和attr的信息方便使用closeIndexBlock(IndexBlock*) 这个函数
    std::string attrName;
    int blockNo;
    IndexBlock();
};
```