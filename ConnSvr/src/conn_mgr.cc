/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：conn_mgr.cc
*   创 建 者：Jeson Yang
*   创建日期：2018年08月20日
*	 描述:		
*
================================================================*/


#include "conn_mgr.h"
#include "channel_def.h"
#include "socket_manager.h"
#include "config_reader.h"
#include "conn_client_mgr.h"
#include "os_time.h"
#include "conn_head.h"

ConnMgr::ConnMgr()
:socket_manager_(NULL), cur_seq_(0), server_seq_(time(NULL)),is_shut_down_(false)
{

}

ConnMgr::~ConnMgr()
{
}

bool ConnMgr::InitConfig(const char* file_name)
{
	string path = file_name;

	ConfigReader reader;
	if (reader.SetSource(path.c_str()))
	{
		printf("can't open config file %s\n", path.c_str());
		return false;
	}

	config_.max_accept_client = reader.GetValueAsUInt("ClientSocketTimeout", 5000);	
	config_.client_socket_timeout = reader.GetValueAsUInt("ClientSocketTimeout", 90);
	config_.wait_reconnect_timeout = reader.GetValueAsUInt("WaitReconnectTimeout", 90);

	config_.listen_client_ip = reader.GetValueAsString("ListenClientIP");
	config_.listen_port_count = reader.GetValueAsUInt("ListenPortCount");
	for (int i = 0; i < config_.listen_port_count; ++i)
	{
		char port[64] = { 0 };
		snprintf(port, sizeof(port), "ListenPort%d", i + 1);

		config_.listen_ports[i] = reader.GetValueAsString(port);
	}
	config_.tcp_send_buf_size = reader.GetValueAsInt("TcpSendBufSize");

	config_.max_cached_send_buff_size = reader.GetValueAsInt("ClientSocketTimeout", 5 * 1024); //5k发送缓冲区
	config_.max_cached_send_time_ms = reader.GetValueAsInt("ClientSocketTimeout", 20);
	return true;
}

bool ConnMgr::InitChannel()
{
	if (channel_.Create(CHANNEL_KEY_C2G, CHANNEL_KEY_G2C, CHANNEL_SIZE_C2G, CHANNEL_SIZE_G2C))
	{
		return false;
	}
	return true;
}

bool ConnMgr::InitSocket()
{
	socket_manager_ = SocketManager::Create(config_.max_accept_client);
	if (NULL == socket_manager_)
	{
		printf("create socketmanager failed!\n");
		return false;
	}

	const int send_buff_len = 1024 * 1024;
	const int recv_buff_len = 500 * 1024;
	const int max_send_pkg_len = 400 * 1024;
	const int max_recv_pkg_len = 100 * 1024;

	for (int i = 0; i < config_.listen_port_count; ++i)
	{
		hconn_t h = socket_manager_->Listen(config_.listen_client_ip.c_str(), config_.listen_ports[i].c_str(),
			this, send_buff_len, recv_buff_len, max_send_pkg_len, max_recv_pkg_len);
		if (INVALID_HCONN == h)
		{
			printf("create listen tcp socket failed, ip='%s', port=%s", config_.listen_client_ip.c_str(), config_.listen_ports[i].c_str());
			return false;
		}
		printf("listen %s:%s\n", config_.listen_client_ip.c_str(), config_.listen_ports[i].c_str());

		socket_manager_->SetActiveTimeout(h, config_.client_socket_timeout);
		socket_manager_->SetRecvTimeout(h, config_.client_socket_timeout);
		socket_manager_->SetCacheSendInfo(h, config_.max_cached_send_buff_size, config_.max_cached_send_time_ms);
	}

	return true;
}

time_t ConnMgr::GetTime()
{
	return 0;//jy::time sys
}
void ConnMgr::CloseSocket(hconn_t s)
{
	socket_manager_->Close(s);
}

void ConnMgr::UpdateSocket()
{
	socket_manager_->Update();

	//socket_manager_->GetHconnCount();
}

void ConnMgr::CheckDisconnectTimeOut()
{
	static int64 last_time = GetTimeMs();
	int64 cur_time = GetTimeMs();

	if (cur_time - last_time < 1000)
	{
		return;
	}

	last_time = cur_time;
	int disconnect_count = 0;
	vector<ClientInfo*> timeout_arr;
	ConnClientMgr::Instance().CollectTimeOutClient(timeout_arr, disconnect_count);

	for (unsigned int i = 0; i < timeout_arr.size(); i++)
	{
		ClientInfo *client = timeout_arr[i];
		//notify close
		
	}
}

void ConnMgr::HandlerPkgFromZone()
{
	static int MAX_MSG_PROC_ONCE = 100;
	static char buf[MAX_SERVER_PACK_LEN + sizeof(ConnMsgHead) + 10240];	//10240预留着
	unsigned int node_len = 0;
	int proc_msg_count = 0;
	int conn_svr_head_len = 0;
	while (true)
	{
		if (proc_msg_count >= MAX_MSG_PROC_ONCE && MAX_MSG_PROC_ONCE > 0)
		{
			break;
		}

		if (!channel_.RecvMgr(buf, node_len))
		{
			break;
		}

		++proc_msg_count;
		conn_svr_head_len = 0;
		static 

	}

}

void ConnMgr::Update()
{
	UpdateSocket();
	CheckDisconnectTimeOut();

}

void ConnMgr::HandleClose(SocketManager *manager, hconn_t s, int close_reason)
{

}

void ConnMgr::HandleConnect(SocketManager *manager, hconn_t s)
{

}

bool ConnMgr::CanAccept(SocketManager *manager, hconn_t listener, TcpSocket &ns)
{
	return true;
}

void ConnMgr::HandleAccept(SocketManager *manager, hconn_t listener, hconn_t ns)
{

}
