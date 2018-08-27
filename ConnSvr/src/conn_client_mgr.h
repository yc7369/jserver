

#pragma onece

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
private:
	ConnMap conn_maps_;
	GameMap game_maps_;

};