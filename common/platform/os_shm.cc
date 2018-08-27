/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：os_shm.cc
*   创 建 者：Jeson Yang
*   创建日期：2018年08月23日
*	 描述:		
*
================================================================*/


#include "os_shm.h"

#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

void *GetShm_(int key, size_t size, int flag, int &id)
{
	int shm_id;
	void *shm;
	if((shm_id = shmget(key, size, flag)) == -1)
	{
		return NULL;
	}

	shm = shmat(shm_id, NULL, 0);
	if(shm == (void*)-1)
	{
		return NULL;
	}
	id = shm_id;
	return shm;
}

void *GetShm(int key, size_t size, bool force_create, int &id)
{
	void *shm;
	shm = GetShm_(key, size, 0666, id);
	if(NULL == shm && !force_create)
	{
		return NULL;
	}

	shm = GetShm_(key, size, 0666 | IPC_CREAT, id);
	return shm;
}

void *GetShmEx(int key, size_t size, bool &if_create, int &id)
{
	int shm_id = shmget(key, size, IPC_CREAT | IPC_EXCL | 0666);
	if(shm_id < 0)
	{
		if(errno != EEXIST)
		{
			return NULL;
		}

		shm_id = shmget(key, 0, 0666);
		if(shm_id < 0)
		{
			return NULL;
		}
		if_create = false;
	}
	else
	{
		if_create = true;
	}

	void *addr = shmat(shm_id, NULL, 0);
	if(addr == (void*)-1)
	{
		return NULL;
	}
	id = shm_id;
	return addr;
}
