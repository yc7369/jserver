/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：conn_mgr.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月20日
*	 描述:		
*
================================================================*/


#pragma once

#include "conn_define.h"
#include "socket_event_handler.h"
#include "message_channel.h"

class  SocketManager;

class ConnMgr : public SocketHandler
{
public:
	static ConnMgr& Instance() {static ConnMgr inst; return inst;}

	ConnMgr();
	~ConnMgr();

	bool InitConfig(const char* file_name);
	bool InitChannel();
	bool InitSocket();

	void UpdateSocket();
	void Update();

	void CloseSocket(hconn_t s);
	void CloseClientSocket(ClientInfo * client);
	void DisconnClient(ClientInfo * client); //让client 处于断连状态，但可以等待重连
	void DeleteClient(ClientInfo * client);  //删除client

	uint64 IncGameId() { return ++cur_game_id_ ; }

    virtual void HandleClose(SocketManager *manager, hconn_t s, int close_reason); 
    virtual void HandleConnect(SocketManager *manager, hconn_t s);
    virtual bool CanAccept(SocketManager *manager, hconn_t listener, TcpSocket &ns);
    virtual void HandleAccept(SocketManager *manager, hconn_t listener, hconn_t ns);

private:

	MessageChannel channel_;
	SocketManager *socket_manager_;
	ConnConfig config_;

	unsigned int cur_seq_;
	unsigned int server_seq_;
	uint64 cur_game_id_;
	bool is_shut_down_;
};
