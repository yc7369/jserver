/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：zone_config.cc
*   创 建 者：Jeson Yang
*   创建日期：2018年07月31日
*   描    述：
*
================================================================*/


#include "zone_config.h"
#include "config_reader.h"
#include "os_fs.h"

ZoneConfig g_zone_config;

bool LoadConfig(const char* file_name)
{
	ConfigReader reader;
	if(!reader.SetSource(file_name))
	{
		printf("can't open config file %s!", file_name);
		return false;
	}

	g_zone_config.root_path = reader.GetValueAsString("RootPath");
	SetDefaultRootPath(g_zone_config.root_path);


//	g_zone_config.log_type = reader.GetrValueAsInt("LogType", );


	g_zone_config.listen_conn_ip = reader.GetValueAsString("ListenConnIP");
	g_zone_config.listen_conn_ip = reader.GetValueAsString("ListenConnPort");

	return true;
}
