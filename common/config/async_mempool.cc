/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：async_mempool.cc
*   创 建 者：Jeson Yang
*   创建日期：2018年07月31日
*   描    述：
*
================================================================*/

#pragma warning(disable: 4996)

#include "async_mempool.h"


AsyncMemPool::PoolChunk::PoolChunk(size_t _size):
    pool(NULL), pos(0), size(_size)
{
    pool = ::malloc(size);
}

AsyncMemPool::PoolChunk::~PoolChunk()
{
    ::free(pool);
}

AsyncMemPool::AsyncMemPool(size_t _defaultSize):
    chunks(NULL), chunksCount(0), defaultSize(_defaultSize),
    poolUsage(0), poolUsageCounter(0)
{
}

AsyncMemPool::~AsyncMemPool()
{    
    for(size_t i = 0; i<chunksCount; i++){
        delete chunks[i];
    }
    ::free(chunks);
}

int AsyncMemPool::initialize()
{
    chunksCount = 1;
    chunks = (PoolChunk**)::malloc(sizeof(PoolChunk*));
    if(chunks == NULL)
        return -1;

    chunks[chunksCount-1] = new PoolChunk(defaultSize);

    return 0;
}

void AsyncMemPool::addNewChunk(size_t size)
{
    chunksCount++;
    chunks = (PoolChunk**)::realloc(chunks, chunksCount*sizeof(PoolChunk*));
    if(size <= defaultSize)
        chunks[chunksCount-1] = new PoolChunk(defaultSize);
    else
        chunks[chunksCount-1] = new PoolChunk(size);
}

void * AsyncMemPool::alloc(size_t size)
{
    PoolChunk * chunk = NULL;
    for(size_t i = 0; i<chunksCount; i++){
        chunk = chunks[i];
        if((chunk->size - chunk->pos) >= size){
            chunk->pos += size;
            return ((char*)chunk->pool) + chunk->pos - size;
        }
    }
    addNewChunk(size);
    chunks[chunksCount-1]->pos = size;
    return chunks[chunksCount-1]->pool;
}

void AsyncMemPool::free()
{    
    size_t pu = 0;
    size_t psz = 0;
    poolUsageCounter++;

    for(size_t i = 0; i<chunksCount; i++){
        pu += chunks[i]->pos;
        psz += chunks[i]->size;
        chunks[i]->pos = 0;
    }
    poolUsage=(poolUsage>pu)?poolUsage:pu;

    if(poolUsageCounter >= 10 && chunksCount > 1){
        psz -= chunks[chunksCount-1]->size;
        if(poolUsage < psz){
            chunksCount--;
            delete chunks[chunksCount];
        }
        poolUsage = 0;
        poolUsageCounter = 0;
    }
}

void * AsyncMemPool::calloc(size_t size)
{
    return ::memset(this->alloc(size), 0, size);
}

char * AsyncMemPool::strdup(const char *str)
{
    return ::strcpy((char*)this->alloc(strlen(str)+1), str);
}

