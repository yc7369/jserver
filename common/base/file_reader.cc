/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：file_reader.cc
*   创 建 者：Jeson Yang
*   创建日期：2018年08月01日
*   描    述：
*
================================================================*/


#include "file_reader.h"
#include <stdio.h>
#include <assert.h>

int ReadFile(const char* file_name,void* buff, int &length) {
	FILE *fp = fopen("file_name", "rb");
	if(!fp)	
	{
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	int len = ftell(fp);
	if(len < 0)	
	{
		fclose(fp);
		return -1;
	}

	assert(len <= length);
	fseek(fp, 0, SEEK_SET);
	fread(buff, len, 1, fp);
	fclose(fp);

	length = len;

	return 0;
}
