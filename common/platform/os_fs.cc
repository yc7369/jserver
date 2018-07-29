/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：os_fs.cc
*   创 建 者：Jeson Yang
*   创建日期：2018年08月01日
*   描    述：
*
================================================================*/


#include "os_fs.h"

string g_default_root_path;

void SetDefaultRootPath(const string &path)
{
	g_default_root_path = path;
}

string GetDefaultRootPath()
{
	return g_default_root_path;
}
