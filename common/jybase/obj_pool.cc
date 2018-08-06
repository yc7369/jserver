/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：obj_pool.cc
*   创 建 者：Jeson Yang
*   创建日期：2018年08月05日
*	 描述:		
*
================================================================*/
#include "stdafx.h"

#include "obj_pool.h"
#include "timer_queue.h"
#include "base_exception.h"

namespace jy
{
	void CObjPoolBase::CreatePool(unsigned int obj_type, size_type unit_size, size_type count, bool resume)
	{
		size_type array_offset = ALIGN_SIZE(sizeof(*this) + count * sizeof(int), 8);
		unit_size = ALIGN_SIZE(unit_size, 8);

		array_.InitArray(reinterpret_cast<char*>(this) + array_offset, unit_size, count);

		if(resume)
		{
			if(unit_size_ != unit_size || count_ != count || obj_type_ != obj_type)
			{
				char err_desc[2048];
				sprintf(err_desc,
					"Invalid memory resume! Original obj type is %u, unit size is "FMT_SIZET", count is "FMT_SIZET"; "
					"Current obj type is %u, unit size is "FMT_SIZET", count is "FMT_SIZET"",
					obj_type_,
					unit_size_,
					count_,
					obj_type,
					unit_size,
					count);
				throw base_exception(err_desc);
				unit_size_ = unit_size;
			}
		}
		else
		{
			unit_size_ = unit_size;
			count_ = count;
			free_node_ = TAIL_OF_LIST;
			max_used_node_ = 0;
			allocate_count_ = 0;
			deallocate_count_ = 0;
			obj_type_ = static_cast<unsigned char>(obj_type);
		}
	}

	CTimerQueue* CObjPoolMgr::CreateTimerQueue4MProtect(size_type count)
	{
		size_type capacity(m_capacity);
		char *Addr2Protect = m_nextPoolBuffer;
		CTimerQueue::CreateTimerQueue(m_nextPoolBuffer, capacity, count, m_resume);
		m_capacity    -= capacity;
		m_nextPoolBuffer += capacity;

#ifndef WIN32
		size_t PageSize = getpagesize();
		char *addr1= (char *)( ((long)Addr2Protect+PageSize-1) /PageSize*PageSize );
		char *addr2= addr1+PageSize;
		int ProtectLen=0;
		while(addr2<m_nextPoolBuffer)
		{
			ProtectLen	+= PageSize;
			addr2		+=PageSize;
		}
		
		if(0==ProtectLen)
		{
			return NULL;
		}

		//WRITE_ERR_LOG("CreateTimerQueue4MProtect [%p][%p][%d]",Addr2Protect,addr1,ProtectLen);
		int ret= mprotect(addr1,ProtectLen,PROT_NONE);
		if(0!=ret)
		{
			perror("mprotect");
			//WRITE_FATAL_LOG("CreateTimerQueue4MProtect, mprotect() Error[%p][%d][%d]",addr1,ProtectLen,errno);
		}
#endif
	
		return NULL;
	}
	CTimerQueue* CObjPoolMgr::CreateTimerQueue( size_type count )
	{
		assert(0 == m_tq);
		size_type capacity(m_capacity);
		m_tq = CTimerQueue::CreateTimerQueue(m_nextPoolBuffer, capacity, count, m_resume);
		if (0 == m_tq)
		{
			return 0;
		}
		m_capacity    -= capacity;
		m_nextPoolBuffer += capacity;
		return m_tq;
	}
}
