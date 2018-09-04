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
#include <sys/shm.h>

#include "world.h"
#include "zone_config.h"
#include "run_flags.h"
#include "world.h"
#include "zone_config.h"

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

int CWorld::Prepare(bool is_resume)
{
	if(DoPrepare(is_resume) != 0)
	{
		if(!is_resume)
		{
			ClearShm();
		}
		return -1;
	}
	
	return 0;
}

int CWorld::DoPrepare(bool is_resume)
{
	if(Initialize(is_resume) != 0)
	{
		return -1;
	}
	
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

CRunFlag g_run_flags;
int CheckRunFlags()
{
    //优先检查 KICK QUIT 模式
    if ( g_run_flags.IsFlagSet(FLAG_QUIT_AFTER_KICK_ALL_USER))
    {
        return FLAG_QUIT_AFTER_KICK_ALL_USER;
    }
    else if( g_run_flags.IsFlagSet( FLAG_QUIT_NORMAL) )
    {
        return FLAG_QUIT_NORMAL;
	}
#if 0
	else if( g_run_flags.IsFlagSet( FLAG_QUIT_NORMAL))
	{
		//todo something.. not return!!!
	}
#endif
    return FLAG_START_NORMAL;
}


int CWorld::Loop()
{
	for( ; ; )
	{
		//time update

		//state check
		if(CheckRunFlags() != FLAG_START_NORMAL)
		{
			break;	
		}
		UpdateSocket();	
	
		//logic
		HandleAsynQueue();	
	}
	return 0;
}

void CWorld::HandleAsynQueue()
{

}
int CWorld::Initialize(bool is_resume)
{
    // 其他初始化失败的信息
    printf("\n\n########### Zone Server Starting ###########################\n\n");

	if(InitChannel() < 0)
	{
		printf("init channel failed!");
		return -1;
	}
	
	if(!InitSocket())
	{
		printf("init socket failed!");
		return -1;
	}
	return 0;
}

int CWorld::InitChannel()
{
	return 0;
}

bool CWorld::InitSocket()
{
	return true;
}


void CWorld::UpdateSocket()
{

}
