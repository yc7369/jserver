/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：message_channel.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月21日
*	 描述:		
*
================================================================*/


#pragma once
#include "message_queue.h"

class MessageChannel
{
public:
	MessageChannel();
	virtual ~MessageChannel();

	bool Create(int send_key, int recv_key, unsigned int send_size, unsigned int recv_size);

	bool RecvMsg(void *buf, unsigned int &length);
	bool SendMsg(const void *buf, unsigned int length);

	bool IsInputEmpty() const { return recv_queue_.IsQueueEmpty();}
	
protected:
	MessageQueue recv_queue_;
	MessageQueue send_queue_;

	void *send_shm_;
	void *recv_shm_;
};
