/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：asyn_queue.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月04日
*	  描述:		
*
================================================================*/


#pragma once

#include "obj_desc.h"

typedef unsigned char BYTE;

#define QUEUE_RESERVE_LENGTH	 8
#define QUEUE_LEN				(2 * 1024 * 1024)
#define MAX_BODY_LEN             1024

enum InterMsgID
{
	INTER_LOGOUT = 1,
	INTER_TIMER = 2,
	
}


struct AsynMsgHead
{
	int msg_id;
	int seq;
	int body_len;
};

strcut AsynMsg
{
	AsynMsgHead msg_head;
	char buffer[MAX_BODY_LEN];
};


//need resume restart
class CAsynQueue
{
public:

	static CAsynQueue& Instance();

	CAsynQueue();
	~CAsynQueue();

	int Initialize();
	int GetHeadCode(BYTE *out_code, int *out_length);

 	struct Head
	{
		int size;
		int offset;
		int begin;
		int end;
	}queue_head_;
	unsigned int seq_;
}
