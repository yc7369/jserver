/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：socket.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月07日
*	 描述:		
*
================================================================*/


#pragma once

#include "socket_def.h"
#include <string>

enum SocketState
{
	SS_INVALID = 0,
	SS_CLOSED = SS_INVALID,
	SS_CLOSING,
	SS_CREATED,
	SS_CONNECTING,
	SS_CONNECTED,
	SS_ACCEPTED = SS_CONNECTED,
	SS_LISTENING,
	SS_UDP,
};

enum SocketCloseReason
{
	SCR_CONNECT_FAIL = 1,
	SCR_HANDLE_CONNECT_FAIL = 2,

	SCR_REMOTE_CLOSE = 3,
	SCR_FD_EXCEPT = 4,
	SCR_CHECK_ACTIVE_TIMEOUT = 5,

	SCR_HANDLE_READ_FAIL = 6,
	SCR_HANDLE_WRITE_FAIL = 7,
	SCR_HANDLE_ACCEPT_FAIL = 8,

	SCR_CHECK_RECV_TIMEOUT = 9,

	SCR_FD_RECV_FAIL = 10,

	SCR_HANDLE_PKG_HEAD_FAIL = 11,

	SCR_CLIENT_PKG_TOO_LONG = 12,

	SCR_SEND_FAILED = 13,

	SCR_INTERNAL = 100,
};

enum SocketFDEvent
{
	SOCK_EVENT_ON_READ = 1,
	SOCK_EVENT_ON_WRITE = 2,
	SOCK_EVENT_ON_EXCEPT = 0x100000,
};

class Socket
{
public:
	Socket();
	virtual ~Socket();

	static std::string IPString(ipaddr_t ip);
	static std::string IPString(ipaddr_t ip, port_t port);
	static ipaddr_t INetAddressIP(const std::string &ip);
	static port_t INetAddressPort(const std::string &port);

	int GetFD() const {return socket_;}
	ipaddr_t GetRemoteIP() const {return remote_ip_;}
	port_t GetRemotePort() const {return remote_port_;}

	bool SetOptNonBlock();
	bool SetOptReuse(bool reuse);

	bool IsValid() const { return SS_INVALID != state_;}

	static int GetProtoByName(const std::string &protocol);	//tcp/ udp /ip..

	bool Create(int fd);
	bool Create(int type, int proto);

	void Close();

	SocketState GetState() const { return state_;}
	void SetState(SocketState state) { state_ = state;}
protected:
	int socket_;
	SocketState state_;
	void *user_data_;
	
	ipaddr_t remote_ip_;
	port_t remote_port_;

	hconn_t hconn_;	//连接标识
};
