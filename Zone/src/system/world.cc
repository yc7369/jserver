/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：world.cc
*   创 建 者：JesonYang
*   创建日期：2018年07月29日
*   描    述：
*
================================================================*/
#include "stdafx.h"

#include "world.h"

CWorld::CWorld()
{
	zone_id_ = -1;
	shm_id_ = -1;
	current_online_num_ = 0;
}

CWorld& CWorld::Instance()
{
	static CWorld g_world;
	return g_world;
}

int CWorld::Prepare(bool isResume)
{
	if(DoPrepare(isResume) != 0)
	{
		if(!isResume)
		{
			ClearShm();
		}
		return -1;
	}
	
	return 0;
}

int CWorld::DoPrepare(bool isResume)
{

	return 0;
}

void CWorld::ClearShm()
{
	if(shm_id_ > 0)
	{
		shmctl(shm_id_, IPC_RMID, NULL);
		//LOG_INFO("#####shm cleaned#######");
	}
}

int CWorld::InitConfig(const char* file_name)
{
	bool ret = LoadConfig(file_name);

	if(!ret)
	{
		return -1;
	}

	return 0;
}

int CWorld::InitLog()
{
	
	return 0;
}
