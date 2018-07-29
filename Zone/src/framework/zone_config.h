/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：zone_config.h
*   创 建 者：Jeson Yang
*   创建日期：2018年07月31日
*   描    述：
*
================================================================*/


#pragma once

#include <string>

struct ZoneConfig
{
	std::string root_path;
	
	int log_type;
	int log_level;
	int log_file_num;
	int log_file_size;
	std::string log_file_path;
	

	std::string server_name;

	std::string listen_conn_ip;	//ConnSvr的IP
	std::string listen_conn_port;	//ConnSvr的端口

	std::string db_ip;
	std::string db_port;
	int is_connect_db;	//是否需要连接db


};

extern ZoneConfig g_zone_config;

bool LoadConfig(const char* file_name);
