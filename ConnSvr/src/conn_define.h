/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：conn_define.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月20日
*	 描述:		
*
================================================================*/


#pragma once
#include "stdafx.h"
#include "socket_def.h"
#include <string>
using namespace std;

enum
{
	MAX_LISTEN_PORT_COUNT = 10,
};

struct ConnConfig
{
	int max_accept_client;
	int client_socket_timeout;
	int wait_reconnect_timeout;
	string listen_client_ip;
	int listen_port_count;
	string listen_ports[MAX_LISTEN_PORT_COUNT];
	int tcp_send_buf_size;

	int max_cached_send_time_ms;// cached send 的时间阀值
	int max_cached_send_buff_size; // cached send 的大小阀值
};

enum
{
	CLIENT_STATE_NORMAL = 1,
	CLIENT_STATE_DISCONNECTED = 2,
	//..
};
struct ClientInfo
{
	unsigned int ip;
	unsigned int port;

	uint64 game_id;	//与zone对应的句柄
	unsigned int create_seq;

	hconn_t hconn_id;
	int state;

    void SetState(int client_state){ state = client_state;}
    int GetState() const{return state;}
};

