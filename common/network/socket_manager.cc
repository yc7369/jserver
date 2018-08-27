/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：socket_manager.cc
*   创 建 者：Jeson Yang
*   创建日期：2018年08月13日
*	 描述:		
*
================================================================*/


#include "socket_manager.h"
#include "epoll_socket_manager.h"
#include <stdlib.h>
#include "os_time.h"

SocketManager *g_shared_socket_manager = NULL;
time_t SocketManager::cur_time_ = GetTimeSec();
int64 SocketManager::cur_time_ms_ = GetTimeMs();
timeval SocketManager::cur_timeval_ = GetTimeVal();

SocketManager* SocketManager::GetSharedManager()
{
	return g_shared_socket_manager;
}
SocketManager* SocketManager::Create(int max_fds)
{
	SocketManager *socket_manager = new EpollSocketManager;

	if(socket_manager->Initialize(max_fds))
	{
		delete socket_manager;
		return NULL;
	}
	return socket_manager;
}

SocketManager::SocketManager()
:max_fds_(0), wait_event_time_(1),
flush_cached_send_sockets_interval_ms_(50), cur_conn_(1)
{
	g_shared_socket_manager = this;
}

SocketManager::~SocketManager()
{
	g_shared_socket_manager = NULL;
}

hconn_t SocketManager::Listen(const char *ip, const char *port, SocketHandler *handle, int send_buf_len, int recv_buf_len, int max_len_per_send_pkg, int max_len_per_recv_pkg)
{
	ipaddr_t n_ip = inet_addr(ip);
	port_t n_port = htons(atol(port));

	return Listen(n_ip, n_port, handle, send_buf_len, recv_buf_len, max_len_per_send_pkg, max_len_per_recv_pkg);
}

hconn_t SocketManager::Listen(ipaddr_t ip, port_t port, SocketHandler *handle, int send_buf_len, int recv_buf_len, int max_len_per_send_pkg, int max_len_per_recv_pkg)
{
	return 0;
}

bool SocketManager::Initialize(int max_fds)
{
	max_fds_ = max_fds + RESERVE_FD_COUNT;
	return true;
}

TcpSocket* SocketManager::GetSocketByHconn(hconn_t hconn)
{
	SocketHconnMap::iterator it = socket_hconn_map_.find(hconn);
	if(it != socket_hconn_map_.end())
	{
		return it->second;
	}
	return NULL;
}

TcpSocket* SocketManager::GetSocketByFD(int fd)
{
	SocketFDMap::iterator it = socket_fd_map_.find(fd);
	if(it != socket_fd_map_.end())
	{
		return it->second;
	}
	return NULL;
}

bool SocketManager::ProcessAccept(TcpSocket &listener)
{
	return true;
}

bool SocketManager::ProcessConnect(TcpSocket &s)
{
	return true;
}

bool SocketManager::ProcessTcpRead(TcpSocket &s)
{
	return true;
}

bool SocketManager::ProcessTcpWrite(TcpSocket &s)
{
	return true;
}

void SocketManager::CheckSocketEvents()
{
	if( !WaitForSocketEvents(wait_event_time_))
	{
		return;
	}

	hconn_t hconn;
	int event_flag;

	while(GetSocketEvent(hconn, event_flag))
	{
		TcpSocket *s = GetSocketByHconn(hconn);
		if(NULL == s)
		{
			continue;
		}
		
		if(s->IsValid() || s->GetState() == SS_CLOSING)
		{
			continue;
		}

		if(event_flag & SOCK_EVENT_ON_EXCEPT)
		{
			//close s
			continue;
		}
		
		if(SS_CONNECTING == s->GetState())
		{
			if(event_flag & SOCK_EVENT_ON_WRITE)
			{
				ProcessConnect(*s);
				continue;
			}
		}
		if(SS_LISTENING == s->GetState())
		{
			if(event_flag & SOCK_EVENT_ON_READ)
			{
				ProcessAccept(*s);
				continue;
			}
		}
		
		if(SS_CONNECTED == s->GetState() && (event_flag & SOCK_EVENT_ON_READ))
		{
			ProcessTcpRead(*s);
		}
		if(SS_CONNECTED == s->GetState() && (event_flag & SOCK_EVENT_ON_WRITE))
		{
			ProcessTcpWrite(*s);
		}
	}
}

void SocketManager::CheckSocketTimeOut()
{
	static time_t last_time = 0;//cur_time_ms_;
	time_t cur_time = 0;//cur_time_ms_;

	if(cur_time - last_time < 1120)
	{
		return ;
	}
	
	last_time = cur_time;

	SocketHconnMap::iterator it = socket_hconn_map_.begin();
	for(; it != socket_hconn_map_.end(); ++it)
	{
		TcpSocket *s = it->second;
		
		if(s->GetState() != SS_CONNECTED)
			continue;

		if(s->GetActiveTimeOut() != 0 && cur_time - s->GetLastActiveTime() > s->GetActiveTimeOut())
		{
			//close s
			continue;
		}

		if(s->GetRecvTimeOut() != 0 && cur_time - s->GetLastRecvTime() > s->GetRecvTimeOut())
		{
			//close s
			continue;
		}

	}
}

void SocketManager::Close(hconn_t hconn)
{
    TcpSocket *s = GetSocketByHconn(hconn);
    if (NULL == s)
    {
        return;
    }
    if (s->GetState() == SS_CLOSING || s->GetState() == SS_CLOSED)
    {
        return;
    }
    s->SendCache(); 

    s->SetState(SS_CLOSING);
    closing_sockets_.push_back(hconn);
}

void SocketManager::SysCloseSocket(TcpSocket &s, int close_reason)
{
	if (s.GetState() == SS_CLOSING || s.GetState() == SS_CLOSED)
	{
		return;
	}

	s.SetState(SS_CLOSING);
	closing_sockets_.push_back(s.GetConn());

	SocketHandler *event_handler = s.GetHandler();
	if (event_handler)
	{
		event_handler->HandleClose(this, s.GetConn(), close_reason);
	}
}

void SocketManager::SetActiveTimeout(hconn_t hconn, int timeout)
{
	TcpSocket *s = GetSocketByHconn(hconn);
	if (NULL == s)
	{
		return;
	}
	s->SetActiveTimeOut(timeout);
}

void SocketManager::SetRecvTimeout(hconn_t hconn, int timeout)
{
	TcpSocket *s = GetSocketByHconn(hconn);
	if (NULL == s)
	{
		return;
	}
	s->SetRecvTimeOut(timeout);
}

void SocketManager::SetCacheSendInfo(hconn_t hconn, int max_cached_send_buff_len, int max_cached_send_interval_ms)
{
	TcpSocket *s = GetSocketByHconn(hconn);
	if (NULL == s)
	{
		return;
	}
	if (max_cached_send_interval_ms < flush_cached_send_sockets_interval_ms_)
	{
		flush_cached_send_sockets_interval_ms_ = max_cached_send_interval_ms; //刷新时间以下的为准
	}
	s->SetCachedSendInfo(max_cached_send_buff_len, max_cached_send_interval_ms);
}

int SocketManager::GetHconnCount() const
{
	return socket_hconn_map_.size();
}

void SocketManager::UpdateTime()
{
	cur_time_ = GetTimeSec();
	cur_time_ms_ = GetTimeMs();
	cur_timeval_ = GetTimeVal();
}

void SocketManager::FlushCachedSendSockets()
{
	static int g_last_time = cur_time_ms_;
	int cur_time = cur_time_ms_;

	if (cur_time - g_last_time < flush_cached_send_sockets_interval_ms_)
	{
		return;
	}

	g_last_time = cur_time;
	for (SocketHconnMap::iterator it = socket_hconn_map_.begin(); it != socket_hconn_map_.end(); ++it)
	{
		TcpSocket &s = *it->second;

		if(s.GetState() != SS_CONNECTED)
		{
			continue;
		}

		if (s.SendCache() < 0)
		{
			SysCloseSocket(s, SCR_SEND_FAILED);
			continue;
		}
	}

}

void SocketManager::DetachSocket(TcpSocket *s)
{
	if (NULL == s)
	{
		return;
	}

	UnregistSocketEvent(*s);

	socket_hconn_map_.erase(s->GetConn());
	socket_fd_map_.erase(s->GetFD());
}

void SocketManager::DoCloseSockets()
{
	for (unsigned int i = 0; i < closing_sockets_.size(); ++i)
	{
		hconn_t hconn = closing_sockets_[i];
		TcpSocket *s = GetSocketByHconn(hconn);
		if (s)
		{
			DetachSocket(s);
			delete s;
		}
	}
}

void SocketManager::Update()
{
	UpdateTime();

	FlushCachedSendSockets();

	CheckSocketEvents();
	
	CheckSocketTimeOut();
}