/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：conn_svr.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月19日
*	 描述:		
*
================================================================*/


#pragma once

#ifndef SERVER_VERSION
#define SERVER_VERSION  "0.1"
#endif

struct CommandLineParams
{
	char config_file[256];
	bool daemon;
};

class ConnServer
{
public:
	ConnServer();
	~ConnServer();

	bool Init(const CommandLineParams &cmd_param);
	void Run();

private:
	bool InitSocket();
	bool InitChannel();
	bool InitConfig(const char* config_file);
	
	bool Stop();
	void Update();
	
};
