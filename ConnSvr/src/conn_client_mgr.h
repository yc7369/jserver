/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*
*   文件名称：conn_client_mgr.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月26日
*	 描述:
*
================================================================*/

#pragma once

#include"conn_define.h"
#include <map>
using namespace std;

typedef map<unsigned int, ClientInfo*> ConnMap;
typedef map<uint64, ClientInfo*> GameMap;

class ConnClientMgr
{
public:
	static ConnClientMgr& Instance()
	{
		static ConnClientMgr inst;
		return inst;
	}

	int GetClientCount() const;

	ClientInfo *GetClientByConnId(unsigned int conn_id);
	ClientInfo *GetClientByConnId(uint64 game_id);

	void AddToConnMap(unsigned int conn_id, ClientInfo* client);
	void AddToGameMap(uint64 game_id, ClientInfo* client);

	void EraseConnMap(unsigned int key);
	void EraseGameMap(uint64 key);

	void CollectTimeOutClient(vector<ClientInfo*> &timeout_arr, int disconnect_count);
private:
	ConnMap conn_maps_;
	GameMap game_maps_;

};