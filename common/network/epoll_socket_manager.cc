/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：epoll_socket_manager.cc
*   创 建 者：Jeson Yang
*   创建日期：2018年08月14日
*	 描述:		
*
================================================================*/


#include "epoll_socket_manager.h"
#include <assert.h>
#include <string.h>

EpollSocketManager::EpollSocketManager()
: ep_fd_(-1), events_(NULL)
{
}

EpollSocketManager::~EpollSocketManager()
{
	delete [] events_;
	events_ = NULL;

	close(ep_fd_);
}

bool EpollSocketManager::Initialize(int max_fds)
{
	SocketManager::Initialize(max_fds);

	if(!(events_ = new epoll_event[max_fds_]))
	{
		assert(false);
		return false;
	}

	if((ep_fd_ = epoll_create(max_fds_)) < 0)
	{
		assert(false);
		return false;
	}


	return true;
}

bool EpollSocketManager::WaitForSocketEvents(int timeout)
{
	nfds_ = epoll_wait(ep_fd_, events_, max_fds_, timeout);

	if(nfds_ <= 0) return false;

	cur_fd_index_ = 0;

	return true;
}

bool EpollSocketManager::GetSocketEvent(hconn_t &hconn, int &event_flag)
{
	assert(events);

	if(cur_fd_index_ >= nfds_)
	{
		return false;
	}

	epoll_event *event = events_ + cur_fd_index_;
	++cur_fd_index_;

	int fd = event->data.fd;
	TcpSocket *s = GetSocketByFD(fd);
	if(NULL == s)
	{
		return false;
	}

	hconn = s->GetConn();

	event_flag = 0;
	if(event->events & EPOLLERR)
	{
		event_flag |= SOCK_EVENT_ON_EXCEPT;
	}
	if(event->events & EPOLLIN)
	{
		event_flag |= SOCK_EVENT_ON_READ;
	}
	if(event->events & EPOLLOUT)
	{
		event_flag |= SOCK_EVENT_ON_WRITE;
	}
	return true;
}

bool EpollSocketManager::UnregistSocketEvent(Socket &s)
{
	if(!s.IsValid()) return false;

	epoll_event event;
	event.data.fd = s.GetFD();

	if(epoll_ctl(ep_fd_, EPOLL_CTL_DEL, s.GetFD(), &event))
	{
		assert(false);
		return false;
	}
	return true;
}

bool EpollSocketManager::RegisterSocketEvent(Socket &s, int event_flag)
{
	if(!s.IsValid())
	{
		return false;
	}

	epoll_event event;
	memset(&event, 0, sizeof(event));

	event.data.fd = s.GetFD();
	event.events = EPOLLHUP | EPOLLERR;

	if(event_flag & SOCK_EVENT_ON_READ)
	{
		event.events |= EPOLLIN;
	}
	if(event_flag & SOCK_EVENT_ON_WRITE)
	{
		event.events |= EPOLLOUT;
	}

	if(epoll_ctl(ep_fd_, EPOLL_CTL_MOD, s.GetFD(), &event) < 0)
	{
		if(errno != ENOENT || epoll_ctl(ep_fd_, EPOLL_CTL_ADD, s.GetFD(), &event) < 0)
		{
			return false;
		}
	}
	return true;
}
