/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：async_mempool.h
*   创 建 者：Jeson Yang
*   创建日期：2018年07月31日
*   描    述：
*
================================================================*/


#pragma once


#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#undef free
#undef calloc

class AsyncMemPool
{
private:
    struct PoolChunk {
        void * pool;
        size_t pos;
        size_t size;

        PoolChunk(size_t _size);
        ~PoolChunk();
    };
    PoolChunk ** chunks;
    size_t chunksCount;
    size_t defaultSize;

    size_t poolUsage;
    size_t poolUsageCounter;

    void addNewChunk(size_t size);

public:
    AsyncMemPool(size_t _defaultSize = 4096);
    virtual ~AsyncMemPool();

    int initialize();
    void free();
    void * alloc(size_t size);
    void * calloc(size_t size);
    char * strdup(const char *str);
};

