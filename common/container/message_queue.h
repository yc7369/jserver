/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：message_queue.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月21日
*	 描述:		
*
================================================================*/


#pragma once

class MessageQueue
{
public:
	MessageQueue();
	~MessageQueue();

	inline bool IsQueueEmpty() const { return queue_head_->tail_idx == queue_head_->head_idx + QUEUE_VACUUM_SLOT;}
	void Attach(void *buf, unsigned int size, bool init_head);
	bool RecvMsg(void *buf, unsigned int &length);
	bool SendMsg(const void *buf, unsigned int length);
	
protected:
	struct QueueHead
	{
		int head_idx;
		int tail_idx;
		int size;
	};

	inline int GetLeftSize(int head_idx, int tail_idx) const;
	inline unsigned int GetSize(int head_idx, int tail_idx) const;

	int GetHeadIdx() const { return queue_head_->head_idx;}
	int GetTailIdx() const { return queue_head_->tail_idx;}
	void SetHeadIdx(int index) {queue_head_->head_idx = index;}
	void SetTailIdx(int index) {queue_head_->tail_idx = index;}

protected:
	enum
	{
		QUEUE_VACUUM_SLOT = 8,
	};
	QueueHead *queue_head_;
	unsigned char  *queue_buff_;
};
