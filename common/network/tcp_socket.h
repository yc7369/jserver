/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：tcp_socket.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月08日
*	 描述:		
*
================================================================*/


#pragma once

#include "socket_def.h"
#include "socket.h"

class SocketHandler;

class TcpSocket : public Socket
{
public:
	enum
	{
		MAX_USER_DATA = 5,
	};

public:
	TcpSocket(int send_buff_len, int recv_buff_len, int max_len_per_send_pkg, int max_len_per_recv_pkg);
	virtual ~TcpSocket();

	bool Create();
	bool Create(int fd){ return Socket::Create(fd);}

	virtual void Close();

	bool IsClosing() const { return SS_CLOSING == state_; }

	int Send(const void *buf, int len, int flags = MSG_NOSIGNAL);
	int Recv(void *buf, int len, int flags = MSG_NOSIGNAL);
	int Recv();

	TcpSocket *Accept();
	bool Connect(const char* ip, const char* port);
	bool Connect(ipaddr_t ip, port_t port);

	bool SetOptKeepAlive(bool keep_alive);
	bool SetOptNoDelay(bool no_delay);
	bool SetOptLinger();
	bool SetOptSendBufSize(unsigned int size);
	bool InitRecvBuf(unsigned int size);

	void UpdateActiveTime();
	void UpdateRecvTime();

	bool CheckConnect(TcpSocket &s);
	bool Listen(ipaddr_t ip, port_t port);

	int GetEventFlag() const {return event_flag_;}
	void SetEventFlag(int event_flag){ event_flag_ = event_flag;}

	void SetHandle(SocketHandler *handler) { event_handler_ = handler;}
	SocketHandler* GetHandler() { return event_handler_;}
	time_t GetLastActiveTime() const { return last_active_time_; }
	time_t GetLastRecvTime() const { return last_recv_time_; }

	void SetActiveTimeOut(time_t t) { active_time_out_ = t;}
	time_t GetActiveTimeOut() { return active_time_out_;}
 	void SetRecvTimeOut(time_t t) { recv_time_out_ = t;}
	time_t GetRecvTimeOut() { return recv_time_out_;}

	std::string RemoteIPString();

	int SendCache();
	void SetCachedSendInfo(int max_cached_send_buf_len, int max_cached_send_interval_ms);

	void SetRemoteAddr(ipaddr_t ip, port_t port);

	void SetConn(hconn_t hconn) { hconn_ = hconn; }
	hconn_t GetConn() const { return hconn_; }
protected:
	void AddCache(const void* buff, int len);

	bool DoConnect(ipaddr_t ip, port_t port);
	bool DoListen();
protected:
	bool support_cached_send_;
	char *cached_send_buf_;	//待发数据，一般需要拼包
	int cur_cached_send_buf_size_;
	int max_cached_send_buf_size_;
	int max_cached_send_interval_ms_;
//	time_t last_cached_send_time_ms_;

	char *recv_buf_;
	int recv_start_;
	int recv_bytes_;

	const int send_buf_len_;
	const int recv_buf_len_;
	int max_len_per_send_pkg_;
	int max_len_per_recv_pkg_;

	SocketHandler *event_handler_;
	int event_flag_;
	time_t last_active_time_;
	time_t active_time_out_;
	time_t last_recv_time_;
	int recv_time_out_;

	hconn_t hconn_;
};
