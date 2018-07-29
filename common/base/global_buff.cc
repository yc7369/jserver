/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：global_buff.cc
*   创 建 者：Jeson Yang
*   创建日期：2018年08月01日
*   描    述：
*
================================================================*/


#include "global_buff.h"

char* GetGlobalBuffer() {
    static char b[GLOBAL_BUFF_SIZE];
    return b;
}
char* GetZeroGlobalBuffer() {
    char* b = GetGlobalBuffer();
    if(b == NULL) {
        return NULL;
    }
    memset(b, 0, GLOBAL_BUFF_SIZE);
    return b;
}
