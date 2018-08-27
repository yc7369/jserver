/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：socket_manager.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月13日
*	 描述:		
*
================================================================*/


#pragma once

#include "socket_def.h"
#include "socket.h"
#include "tcp_socket.h"
#include "socket_event_handler.h"
#include <map>
#include <vector>
using namespace std;

class SocketManager
{
public:
	static SocketManager *Create(int max_fds);

	static time_t cur_time_;
	static int64 cur_time_ms_;
	static timeval cur_timeval_;

	static time_t GetCurTime() { return cur_time_;}
	static int64 GetCurTimeMs() { return cur_time_ms_;}
	static timeval GetCurTimeVal() { return cur_timeval_;}

private:
	enum { RESERVE_FD_COUNT = 30, };

public:
	static SocketManager *GetSharedManager();

	SocketManager();
	virtual ~SocketManager();


	hconn_t Listen(const char *ip, const char *port, SocketHandler *handle, int send_buf_len, int recv_buf_len, int max_len_per_send_pkg, int max_len_per_recv_pkg);
	hconn_t Listen(ipaddr_t ip, port_t port, SocketHandler *handle, int send_buf_len, int recv_buf_len, int max_len_per_send_pkg, int max_len_per_recv_pkg);

	void Close(hconn_t hconn);
	void SysCloseSocket(TcpSocket &s, int closeReason);

    TcpSocket *GetSocketByHconn(hconn_t hconn);    
    TcpSocket *GetSocketByFD(HSOCKET fd);

	void SetActiveTimeout(hconn_t hconn, int timeout);
	void SetRecvTimeout(hconn_t hconn, int timeout);
	void SetCacheSendInfo(hconn_t hconn, int max_cached_send_buff_len, int max_cached_send_interval_ms);

	int GetHconnCount() const;

	virtual void Update();
protected:
	virtual bool Initialize(int max_fds);

	virtual bool WaitForSocketEvents(int timeout) = 0;
	virtual bool GetSocketEvent(hconn_t &hconn, int &event_flag) = 0;

    virtual bool RegisterSocketEvent(Socket &s, int event_flag) = 0;
    virtual bool UnregistSocketEvent(Socket &s) = 0;

    bool ProcessAccept(TcpSocket &listener);
    bool ProcessConnect(TcpSocket &s);
    bool ProcessTcpRead(TcpSocket &s);
    bool ProcessTcpWrite(TcpSocket &s);

	void CheckSocketEvents();
	void CheckSocketTimeOut();

	void UpdateTime();

	void FlushCachedSendSockets();
	void DoCloseSockets();

	void DetachSocket(TcpSocket *s);

protected:
	typedef map<hconn_t, TcpSocket*> SocketHconnMap;
	typedef map<int, TcpSocket*> SocketFDMap;
	typedef std::vector<hconn_t> CloseingSocketMap;

	SocketHconnMap socket_hconn_map_;
	SocketFDMap socket_fd_map_;
	CloseingSocketMap closing_sockets_;

	unsigned int max_fds_;
	int wait_event_time_;
	int flush_cached_send_sockets_interval_ms_;

	hconn_t cur_conn_;
};
