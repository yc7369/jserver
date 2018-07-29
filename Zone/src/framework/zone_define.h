/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：zone_define.h
*   创 建 者：JesonYang
*   创建日期：2018年07月29日
*   描    述：
*
================================================================*/


#pragma once

#ifndef WIN32
#else
#define strcasecmp _stricmp
#endif



#ifndef SERVER_VERSION
#define SERVER_VERSION "0.1"
#endif

struct CommandLineParams
{
	char config_file[256];
	bool is_daemon;
	bool is_resume;
};
