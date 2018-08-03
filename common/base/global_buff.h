/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：global_buff.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月01日
*   描    述：
*
================================================================*/


#pragma once
#include <string.h>

#define GLOBAL_BUFF_SIZE (16 * 1024 * 1024)

char* GetGlobalBuffer();
char* GetZeroGlobalBuffer();
