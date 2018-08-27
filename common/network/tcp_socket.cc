/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：tcp_socket.cc
*   创 建 者：Jeson Yang
*   创建日期：2018年08月08日
*	 描述:		
*
================================================================*/


#include "tcp_socket.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>

TcpSocket::TcpSocket(int send_buff_len, int recv_buff_len, int max_len_per_send_pkg, int max_len_per_recv_pkg) :
support_cached_send_(false), cached_send_buf_(NULL), cur_cached_send_buf_size_(0), max_cached_send_buf_size_(0),
max_cached_send_interval_ms_(0), recv_buf_(0), recv_start_(0), recv_bytes_(0), 
send_buf_len_(send_buff_len), recv_buf_len_(recv_buff_len),
max_len_per_send_pkg_(max_len_per_send_pkg), 
max_len_per_recv_pkg_(max_len_per_recv_pkg),
last_active_time_(0), active_time_out_(0),
last_recv_time_(0), recv_time_out_(0)
{

}

TcpSocket::~TcpSocket()
{
	Close();
}

bool TcpSocket::Create()
{
	int proto = Socket::GetProtoByName("tcp");
	
	return Socket::Create(SOCK_STREAM, proto);
}

void TcpSocket::Close()
{
	
}

int TcpSocket::SendCache()
{
	if(SS_CONNECTED != GetState())
	{
		return -1;
	}

	if(0 == cur_cached_send_buf_size_)
	{
		return 0;
	}

	int n = send(socket_, cached_send_buf_, cur_cached_send_buf_size_, MSG_NOSIGNAL);

	if(n != cur_cached_send_buf_size_)
	{
		return -2;	//没法送完
	}

	cur_cached_send_buf_size_ = 0;

	return 0;
}

void TcpSocket::AddCache(const void* buf, int len)
{
	if(cur_cached_send_buf_size_ + len > max_cached_send_buf_size_)
	{
		//assert(false);
		return;
	}

	memcpy(cached_send_buf_ + cur_cached_send_buf_size_, buf, len);
	cur_cached_send_buf_size_ += len;
}

void TcpSocket::SetCachedSendInfo(int max_cached_send_buf_len, int max_cached_send_interval_ms)
{
	if(max_cached_send_buf_len <= 0)
	{
		return;
	}

	support_cached_send_ = true;
	
	max_cached_send_buf_size_ = max_cached_send_buf_len;
	max_cached_send_interval_ms_ = max_cached_send_interval_ms;

	if(NULL == cached_send_buf_)
	{
		cached_send_buf_ = new char[max_cached_send_buf_size_];
	}
}

std::string TcpSocket::RemoteIPString()
{
	return Socket::IPString(GetRemoteIP(), GetRemotePort());
}

void TcpSocket::UpdateActiveTime()
{
	last_active_time_ = 0;//gametime or XXXtime
}

void TcpSocket::UpdateRecvTime()
{
	last_recv_time_ = 0;//gametime or XXXtime
}

int TcpSocket::Send(const void *buf, int len, int flags)
{
	if(NULL == buf)
	{
		return -1;
	}

	if(GetState() != SS_CONNECTED)
	{
		return -2;
	}

	UpdateActiveTime();

	if(len > max_len_per_send_pkg_)
	{
		//这里有必要加log，为了以后的逻辑故障排查
		//log 
		return -3;
	}

	int n = 0;
	if(support_cached_send_)
	{
		if(len + cur_cached_send_buf_size_ > max_cached_send_buf_size_)
		{
			n = SendCache();
			if(n < 0)
			{
				return -4;
			}

			n = send(socket_, (const char*)buf, len, flags);
			if(n != len)
			{
				return -5;
			}
		}
	}
	else
	{
		n = send(socket_, (const char*)buf, len, flags);
		if(n != len)
		{
			return -6;
		}
	}
	return 0;
}

int TcpSocket::Recv(void *buf, int len, int flags)
{
	UpdateActiveTime();
	UpdateRecvTime();

	if(NULL == buf)
	{
		assert(false);
		return -1;
	}
	int n = recv(socket_, buf, len, flags);
	return n;
}

int TcpSocket::Recv()
{
	UpdateActiveTime();
	UpdateRecvTime();

	int remain_len = recv_buf_len_ - (recv_start_ + recv_bytes_);
	assert(remain_len >= 0);

	int n = ::recv(socket_, recv_buf_ + recv_start_ + recv_bytes_, remain_len, 0);
	if(n > 0)
	{
		recv_bytes_ += n;
	}
	return n;
}

TcpSocket* TcpSocket::Accept()
{
	struct sockaddr_in sa;
	socklen_t length = sizeof(sa);

	int fd = accept(socket_, (struct sockaddr*)&sa, &length);
	if(fd < 0)
	{
		//log_error(accept new socket error :%s, strerror(errno));
		return NULL;
	}

	TcpSocket *s = new TcpSocket(send_buf_len_, recv_buf_len_, max_len_per_send_pkg_, max_len_per_recv_pkg_);
	if(s->Create(fd))	
	{
		delete s;
		return NULL;
	}

	ipaddr_t ip = *((ipaddr_t *)&sa.sin_addr);
	port_t port = sa.sin_port;

	s->SetRemoteAddr(ip, port);
	s->SetState(SS_ACCEPTED);
	
	s->UpdateActiveTime();
	s->UpdateRecvTime();
//	s->SetHandle();
//	s->CopyUserData(*this);
	s->SetCachedSendInfo(max_cached_send_buf_size_, max_cached_send_interval_ms_);

	bool success = false;
	do
	{
		if(!s->SetOptNoDelay(true))
		{
			break;
		}

		if(!s->SetOptNonBlock())
		{
			break;
		}

		if(!s->SetOptLinger())
		{
			break;
		}

		if(!s->SetOptReuse(true))
		{
			break;
		}

		if(!s->InitRecvBuf(recv_buf_len_))
		{
			break;
		}

		if(!s->SetOptSendBufSize(send_buf_len_))
		{
			break;
		}
		success = true;
	}while(false);

	if(!success)
	{
		delete s;
		return NULL;
	}

	return s;
}


bool TcpSocket::Connect(const char* ip, const char* port)
{
	ipaddr_t n_ip = inet_addr(ip);
	port_t n_port = htons(atol(port));

	return Connect(n_ip, n_port);
}

bool TcpSocket::Connect(ipaddr_t ip, port_t port)
{
	if(SS_INVALID == state_)
	{
		if(!Create())
		{
			return false;
		}
	}
	
	SetRemoteAddr(ip, port);

	bool success = false;
	do
	{
		if(!SetOptNoDelay(true))
		{
			break;
		}

		if(!SetOptNonBlock())
		{
			break;
		}

		if(!SetOptLinger())
		{
			break;
		}

		if(!SetOptReuse(true))
		{
			break;
		}

		if(!InitRecvBuf(recv_buf_len_))
		{
			break;
		}

		if(!SetOptSendBufSize(send_buf_len_))
		{
			break;
		}
		
		if(DoConnect(ip, port))
		{
			break;
		}
		success = true;
	}while(false);

	if(!success)
	{
		Close();
		return false;
	}
	return true;
}

bool TcpSocket::SetOptKeepAlive(bool keep_alive)
{
	int optval = keep_alive ? 1 : 0;
	if(setsockopt(socket_, SOL_SOCKET, SO_KEEPALIVE, (char*)&optval, sizeof(optval)) == -1)
	{
		return false;
	}
	return true;
}

bool TcpSocket::SetOptNoDelay(bool no_delay)
{
	int optval = no_delay ? 1 : 0;
	if(setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval)) == -1)
	{
		return false;
	}
	return true;

}

bool TcpSocket::SetOptLinger()
{
	struct linger ling = {0, 0};
	if(setsockopt(socket_, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof(ling)) == -1)
	{
		return false;
	}
	return true;
}

bool TcpSocket::SetOptSendBufSize(unsigned int size)
{
	if(setsockopt(socket_, SOL_SOCKET, SO_SNDBUF, (char*)&size, sizeof(socklen_t)) == -1)
	{
		return false;
	}
	return true;

}

bool TcpSocket::InitRecvBuf(unsigned int size)
{
	if(recv_buf_)
	{
		assert(false);
		return false;
	}

	recv_buf_ = new char[size];
	return (recv_buf_ != NULL);
}

bool TcpSocket::CheckConnect(TcpSocket &s)
{
	int err;
	socklen_t len = sizeof(err);
	getsockopt(s.GetFD(), SOL_SOCKET, SO_ERROR, &err, &len);

	return (0 == err);
}

bool TcpSocket::DoConnect(ipaddr_t ip, port_t port)
{
	struct sockaddr_in sa;
	socklen_t len = sizeof(sa);
	memset(&sa, 0, len);
	sa.sin_family = AF_INET;
	sa.sin_port = port;
	*((unsigned int*)&sa.sin_addr) = ip;

	int ret = connect(socket_, (struct sockaddr*)&sa, len);
	if(-1 == ret)
	{
		if(errno == EINPROGRESS)
		{
			SetState(SS_CONNECTING);
			return true;
		}
	}
	
	SetState(SS_CONNECTED);
	
	UpdateActiveTime();
	UpdateRecvTime();

	return true;
}

bool TcpSocket::Listen(ipaddr_t ip, port_t port)
{
	if(SS_INVALID == state_)	
	{
		if(!Create())
		{
			return false;
		}
	}
	//todo:
	//
	//
	return true;
}

bool TcpSocket::DoListen()
{
	if(0 == listen(socket_, 1024))
	{
		SetState(SS_LISTENING);
		return true;
	}
	return false;
}

void TcpSocket::SetRemoteAddr(ipaddr_t ip, port_t port)
{
	remote_ip_ = ip;
	remote_port_ = port;
}


