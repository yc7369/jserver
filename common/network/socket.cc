/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：socket.cc
*   创 建 者：Jeson Yang
*   创建日期：2018年08月08日
*	 描述:		
*
================================================================*/


#include "socket.h"
#include <stdlib.h>

Socket::Socket()
: socket_(INVALID_SOCKET), state_(SS_INVALID),
user_data_(NULL), remote_ip_(0), remote_port_(0)
{
}


Socket::~Socket()
{
	Close();
}


void Socket::Close()
{
	if(INVALID_SOCKET != socket_)
	{
		close(socket_);
		socket_ = INVALID_SOCKET;
		state_ = SS_CLOSED;
	}
}

bool Socket::SetOptNonBlock()
{
	int old_flag = fcntl(socket_, F_GETFL, 0);
	fcntl(socket_, F_SETFL, old_flag | O_NONBLOCK);

	return true;
}

bool Socket::SetOptReuse(bool reuse)
{
	int optval = reuse ? 1 : 0;
	if(-1 == setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval)))
	{
		return false;
	}
	return true;
}

int Socket::GetProtoByName(const std::string &protocol)
{
	struct protoent *p = NULL;
	if(protocol.size())
	{
		p = getprotobyname(protocol.c_str());
		if(NULL == p)
		{
			return 0;
		}
	}
	return p->p_proto;
}

bool Socket::Create(int fd)
{
	if(INVALID_SOCKET == fd)
	{
		return false;
	}

	socket_ = fd;
	state_ = SS_CREATED;
	return true;
}

bool Socket::Create(int type, int proto)
{
	int fd = socket(AF_INET, type, proto);
	
	if(INVALID_SOCKET == fd)
	{
		return false;
	}

	socket_ = fd;
	state_ = SS_CREATED;
	return true;
}

std::string Socket::IPString(ipaddr_t ip)
{
	std::string str_ip = inet_ntoa(*((in_addr*)&ip));
	return str_ip;
}

std::string Socket::IPString(ipaddr_t ip, port_t port)
{
	char ip_port[256] = {0};
	sprintf(ip_port, "%s:%d", inet_ntoa(*((in_addr*)&ip)), ntohs(port));
	return ip_port;
}

ipaddr_t Socket::INetAddressIP(const std::string &ip)
{
	return inet_addr(ip.c_str());
}

port_t Socket::INetAddressPort(const std::string &port)
{
	return htons((port_t)atoi(port.c_str()));
}
