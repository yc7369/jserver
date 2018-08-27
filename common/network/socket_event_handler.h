/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：socket_event_handler.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月15日
*	 描述:		
*
================================================================*/


#pragma once
#include "tcp_socket.h"
class SocketManager;

class SocketHandler
{
public:
	virtual void HandleClose(SocketManager *manager, hconn_t s, int close_reason) {}
	virtual void HandleConnect(SocketManager *manager, hconn_t s) {}	
	virtual bool CanAccept(SocketManager *manager, hconn_t listener, TcpSocket &ns) { return true; }
	virtual void HandleAccept(SocketManager *manager, hconn_t listener, hconn_t ns) {}
	virtual void HandleWriteEvent(SocketManager *manager, hconn_t s) {}

//	virtual void HandlePkg(SocketManager *manager, hconn_t con, const char* pkg, int len) = 0;
//	virtual int GetPkgLenFromHead(SocketManager *manager, hcon_t con, const char* pkg, int pkg_head_len) = 0;
//	virtual int GetPkgHeadMinLen() = 0;

	virtual ~SocketHandler() {}
};
