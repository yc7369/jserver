/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：pb_config.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月01日
*   描    述：
*
================================================================*/


#pragma once

#include <stdio.h>
#include "common/base/file_reader.h"
#include "common/base/global_buff.h"

#include <google/protobuf/text_format.h>

template<typename T>
class PbConfig
{
private:
	T config_;

public:
	static PbConfig<T> &Instance(){static PbConfig<T> inst; return inst;}

	bool Load(const char* config_file)
	{
		char *file_buff = GetGlobalBuffer();
		int len = GLOBAL_BUFF_SIZE;
		int ret = ReadFile(config_file, file_buff, len);
		if(0 != ret)
		{
			printf("Read Config file error, path = %s\n", config_file);
			return false;
		}

		std::string content(file_buff, len);
		bool r = google::protobuf::TextFormat::ParseFromString(content, config_);
		if(!r)
		{
			printf("Parse config format error!  path = %s buff = %s\n", config_file, content.c_str());
			return false;
		}
		
		printf("Config File %s Read Success!\n", config_file);
	}

	const T& GetConfig()
	{
		return config_;
	}
};
