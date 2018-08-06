/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：asyn_queue.cc
*   创 建 者：Jeson Yang
*   创建日期：2018年08月04日
*	  描述:		
*
================================================================*/


#include "asyn_queue.h"

static CAsynQueue* g_asyn_queue = NULL;

CAsynQueue& CAsynQueue::Instance()
{
	return *g_asyn_queue;
}

CAsynQueue::CAsynQueue()
{

}

int Initialize()
{
	if(jy::IsResume())
	{
	}
	else
	{
		BYTE *buff = NULL;
		
		queue_head_.size_ = QUEUE_LEN;
		queue_head_.begi_ = 0;
		queue_head_.end = 0;

		buff = (BYTE*)this + sizeof(*this);

		if(!buff)
		{
			queue_head_.offset = -1;
			return -1;
		}
		queue_head_.offset = sizeof(*this); 
	}
	return 0;
}

int GetHeadCode(BYTE *out_code, int *out_length)
{
	return 0;
}
