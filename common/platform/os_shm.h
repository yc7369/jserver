/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：os_shm.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月23日
*	 描述:		
*
================================================================*/


#pragma once
#include<stddef.h>

void *GetShm(int key, size_t size, bool create, int &id);
void *GetShmEx(int key, size_t size, bool &create, int &id);
