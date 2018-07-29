/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：world.h
*   创 建 者：JesonYang
*   创建日期：2018年07月29日
*   描    述：
*
================================================================*/


#pragma once

class CWorld
{
public:

	CWorld();
	virtual ~CWorld(){}

	static CWorld& Instance();	

	int Prepare(bool isResume);
	int InitConfig(const char* file_name);
	int InitLog();
	
private:
	int DoPrepare(bool isResume);
	void ClearShm();
	
private:
	time_t game_timer_;
	int zone_id_;
	int shm_id_;
	int current_online_num_;
};