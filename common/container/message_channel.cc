/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：message_channel.cc
*   创 建 者：Jeson Yang
*   创建日期：2018年08月21日
*	 描述:		
*
================================================================*/


#include "message_channel.h"
#include <stdio.h>
#include"common/platform/os_shm.h"

MessageChannel::MessageChannel()
 :send_shm_(NULL), recv_shm_(NULL)
{
}

MessageChannel::~MessageChannel()
{

}

bool MessageChannel::Create(int send_key, int recv_key, unsigned int send_size, unsigned int recv_size)
{
	bool send_clear = false;
	bool recv_clear = false;
	int send_shm_id = 0;
	send_shm_ = GetShmEx(send_key, send_size, send_clear, send_shm_id);
	if (NULL == send_shm_)
	{
		return false;
	}

	int recv_shm_id = 0;
	recv_shm_ = GetShmEx(recv_key, recv_size,recv_clear, recv_shm_id);
	if (NULL == recv_shm_)
	{
		return false;
	}

	send_queue_.Attach(send_shm_, send_size, send_clear);
	recv_queue_.Attach(recv_shm_, recv_size, recv_clear);
	return true;
}

bool MessageChannel::RecvMsg(void *buf, unsigned int &length)
{
	recv_queue_.RecvMsg(buf, length);
	return true;
}

bool MessageChannel::SendMsg(const void *buf, unsigned int length)
{
	send_queue_.SendMsg(buf, length);
	return true;
}

