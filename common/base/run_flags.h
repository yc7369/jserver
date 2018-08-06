/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：run_flags.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月04日
*   描    述：
*
================================================================*/


#pragma once

enum APP_FLAGS
{
	FLAG_START_NORMAL = 0x1 << 0,
	FLAG_START_RESUME = 0x1 << 1,

    //退出方式,留4个bit
    FLAG_QUIT_NORMAL = 0x1 << 4,  //正常退出
    FLAG_QUIT_ABNORMAL = 0x1 << 5,  //非正常退出
    FLAG_QUIT_AFTER_KICK_ALL_USER = 0x1 << 6,     //退出前先强制所有用户下线
	FLAG_QUIT_COREDUMP = 0x1 << 7,

	//运行中控制..
	
};


class CRunFlag
{
public:
	typedef unsigned int FlagType;
	CRunFlag();
	~CRunFlag();

	FlagType GetAllFlag();
	int ClearFlag(FlagType flag);
	int SetFlag(FlagType flag);
	bool IsFlagSet(FlagType flag);
private:
	FlagType flag_;
};
