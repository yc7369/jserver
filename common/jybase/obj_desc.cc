/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：obj_desc.cc
*   创 建 者：Jeson Yang
*   创建日期：2018年08月05日
*	  描述:		
*
================================================================*/


#include "obj_desc.h"
#include "obj_pool.h"

namespace jy
{
	char* tagObjID::g_mem_pool_start = NULL;
	char* tagObjID::g_mem_pool_end = NULL;

	// 用于设置对象生成的方式
	static bool g_resume = false;
	static long g_resume_fix = 0;

	void tagObjIDRannge::Reset(CObjPoolBase *obj)
	{
		pool = obj;
		mem_pool_start = GetOffset(obj->PoolStart());
		unit_size = obj->UnitSize();
		mem_pool_end = mem_pool_start + unit_size * obj->MaxSize();
	}

	bool IsResume()
	{
		return g_resume;
	}


	long ResumeFix()
	{
		return g_resume_fix;
	}
}
