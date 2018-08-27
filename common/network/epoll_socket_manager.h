/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：epoll_socket_manager.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月14日
*	 描述:		
*
================================================================*/


#pragma once

#include "socket_manager.h"
#include <sys/epoll.h>

class EpollSocketManager : public SocketManager
{
public:
	EpollSocketManager();
	virtual ~EpollSocketManager();

private:
	virtual bool Initialize(int max_fds);
	virtual bool UnregistSocketEvent(Socket &s);
	virtual bool RegisterSocketEvent(Socket &s, int eventFlag);

	virtual bool WaitForSocketEvents(int timeout);
	virtual bool GetSocketEvent(hconn_t &hconn, int &event_flag);
	
private:
	int ep_fd_;
	int nfds_;

	struct epoll_event *events_;

	int cur_fd_index_;
};
