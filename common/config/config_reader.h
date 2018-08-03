/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：config_reader.h
*   创 建 者：Jeson Yang
*   创建日期：2018年07月31日
*   描    述：
*
================================================================*/


#pragma once

#include <string>
using namespace std;

class DOTCONFDocument;

class ConfigReader
{
public:
	ConfigReader();
	~ConfigReader();


    bool SetSource(const char *file, bool ignorecase = true);

    std::string GetValueAsString(const char* name, const char* def = "");

    bool GetValueAsBool(const char* name, const bool def = false);

    int GetValueAsInt(const char* name, int def = 0);

	unsigned int GetValueAsUInt(const char *name, unsigned int def = 0);

    float GetValueAsFloat(const char* name, float def = 0);	

private:
    DOTCONFDocument *m_conf;
};
