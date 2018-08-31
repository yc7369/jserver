#include"conn_client_mgr.h"
#include "conn_mgr.h"

int ConnClientMgr::GetClientCount() const
{
	return game_maps_.size();
}

ClientInfo *ConnClientMgr::GetClientByConnId(unsigned int conn_id)
{
	ConnMap::iterator it = conn_maps_.find(conn_id);
	if (it == conn_maps_.end())
	{
		return NULL;
	}
	return it->second;
}

ClientInfo *ConnClientMgr::GetClientByConnId(uint64 game_id)
{
	GameMap::iterator it = game_maps_.find(game_id);
	if (it == game_maps_.end())
	{
		return NULL;
	}
	return it->second;
}

void ConnClientMgr::AddToConnMap(unsigned int conn_id, ClientInfo* client)
{
	conn_maps_[conn_id] = client;
}

void ConnClientMgr::AddToGameMap(uint64 game_id, ClientInfo* client)
{
	game_maps_[game_id] = client;
}

void ConnClientMgr::EraseConnMap(unsigned int key)
{
	conn_maps_.erase(key);
}

void ConnClientMgr::EraseGameMap(uint64 key)
{
	game_maps_.erase(key);
}

void ConnClientMgr::CollectTimeOutClient(vector<ClientInfo*> &timeout_arr, int disconnect_count)
{
	for (GameMap::iterator it = game_maps_.begin(); it != game_maps_.end(); ++it)
	{
		ClientInfo *client = it->second;
		if (CLIENT_STATE_DISCONNECTED == client->GetState())
		{
			++disconnect_count;
			if (ConnMgr::Instance().GetTime() - client->disconnect_time >= ConnMgr::Instance().GetConfig().wait_reconnect_timeout)
			{
				timeout_arr.push_back(client);
			}
		}
	}
}