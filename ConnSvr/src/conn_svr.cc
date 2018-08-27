/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：conn_svr.cc
*   创 建 者：Jeson Yang
*   创建日期：2018年08月19日
*	 描述:		
*
================================================================*/

#include "stdafx.h"
#include "conn_svr.h"
#include "conn_mgr.h"


ConnServer::ConnServer()
{}

ConnServer::~ConnServer()
{}


bool ConnServer::Init(const CommandLineParams &cmd_param)
{
	if(!InitConfig(cmd_param.config_file))
	{
		printf("load config failed!\n");
		return false;
	}

	if(!InitChannel())
	{
		printf("init ConnServer to ZoneServer channel failed!\n");
		return false;
	}


	if(!InitSocket())
	{
		printf("init socket failed!\n");
		return false;
	}

	return true;
}

void ConnServer::Run()
{
	while(true)
	{
		Update();
	}
}

bool ConnServer::InitSocket()
{
	ConnMgr::Instance().InitSocket();

	return true;
}

bool ConnServer::InitChannel()
{
	ConnMgr::Instance().InitChannel();

	return true;
}

bool ConnServer::InitConfig(const char* file_name)
{
	ConnMgr::Instance().InitConfig(file_name);
	return true;
}

void ConnServer::Update()
{
	ConnMgr::Instance().Update();
}
