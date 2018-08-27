/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：message_queue.cc
*   创 建 者：Jeson Yang
*   创建日期：2018年08月21日
*	 描述:		
*
================================================================*/


#include "message_queue.h"
#include <stdio.h>
#include <string.h>

MessageQueue::MessageQueue()
: queue_head_(NULL), queue_buff_{NULL}
{
}

MessageQueue::~MessageQueue()
{
}

void MessageQueue::Attach(void *buf, unsigned int size, bool init_head)
{
	if(size < sizeof(QueueHead) + QUEUE_VACUUM_SLOT)
	{
		return;
	}

	queue_head_ = (QueueHead *)buf;
	queue_buff_ = (unsigned char*)buf + sizeof(QueueHead);
	if(init_head)
	{
		queue_head_->head_idx = 0;
		queue_head_->tail_idx = 0;
		queue_head_->size = size - sizeof(QueueHead);
	}
}

bool MessageQueue::RecvMsg(void *buf, unsigned int &length)
{
	if(NULL == buf)
	{
		return false;
	}

	length = 0;
	int head_idx = GetHeadIdx();
	int tail_idx = GetTailIdx();

	if(head_idx == tail_idx)
	{
		length = 0;
		return false;
	}

	unsigned int size = GetSize(head_idx, tail_idx);
	if(size < sizeof(unsigned int))
	{
		return false;
	}
	
	unsigned char *dstBuf = (unsigned char*)&length;
	for(unsigned int i = 0; i < sizeof(unsigned int); ++i)
	{
		dstBuf[i] = queue_buff_[head_idx];
		head_idx = (head_idx + 1) % queue_head_->size;
	}
	size -= sizeof(unsigned int);
	if(length > (unsigned int) size)
	{
		return false;
	}
	
	if(head_idx + (int)length > queue_head_->size)
	{
		memcpy(buf, queue_buff_ + head_idx, queue_head_->size - head_idx);
		memcpy(buf + queue_head_->size - head_idx, queue_buff_, length - (queue_head_->size - head_idx));
	}
	else
	{
		memcpy(buf, queue_buff_ + head_idx, length);
	}

	head_idx = (head_idx + length) % queue_head_->size;
	SetHeadIdx(head_idx);

	return true;
}

bool MessageQueue::SendMsg(const void *buf, unsigned int length)
{
	if(NULL == buf)
	{
		return false;
	}

	int head_idx = GetHeadIdx();
	int tail_idx = GetTailIdx();

	int left_size = GetLeftSize(head_idx, tail_idx);
	if((unsigned int)left_size < length +  sizeof(unsigned int))
	{
		return false;
	}

	unsigned char *srcBuf = (unsigned char*)&length;
	for(unsigned int i = 0; i < sizeof(unsigned int); ++i)
	{
		queue_buff_[tail_idx] = srcBuf[i];
		tail_idx = (tail_idx + 1) % queue_head_->size;
	}

	if((int)length + tail_idx > queue_head_->size)
	{
		memcpy(queue_buff_ + tail_idx, buf, queue_head_->size - tail_idx);
		memcpy(queue_buff_, buf + queue_head_->size - tail_idx, length - (queue_head_->size - tail_idx));
	}
	else
	{
		memcpy(queue_buff_ + tail_idx, buf, length);
	}

	tail_idx = (tail_idx + length) % queue_head_->size;
	SetTailIdx(tail_idx);
	return true;
}

inline int MessageQueue::GetLeftSize(int head_idx, int tail_idx) const
{
	if(tail_idx >= head_idx)
	{
		return queue_head_->size - tail_idx + head_idx - QUEUE_VACUUM_SLOT;
	}
	return head_idx - tail_idx - QUEUE_VACUUM_SLOT;
}

inline unsigned int MessageQueue::GetSize(int head_idx, int tail_idx) const
{
	if(tail_idx >= head_idx)
	{
		return tail_idx - head_idx;
	}
	return queue_head_->size - head_idx + tail_idx;
}

