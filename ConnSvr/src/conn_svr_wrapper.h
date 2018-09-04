#pragma once

#include"conn.pb.h"
#include"conn_head.h"

class CConnSvrWrapper
{
public:
	CConnSvrWrapper();
	~CConnSvrWrapper();

	int Init();

	int EncodeConnSvrHead(char* buff, int *buff_len, char cmd, uint64 game_id, int cmd_len, int reason);
	int DecodeConnSvrHead(char *buff, int buff_len, int &head_len);

	ConnSvrMsg &GetConnSvrHead() { return msg; }
private:
	ConnSvrMsg msg;

	static Pb::ConnExtStart start;
	static Pb::ConnExtStop stop;
	static Pb::ConnExtBroadcast broadcast;
	static Pb::ConnExtReconn reconn;
	static Pb::ConnExtNotifyRelay notifyRelay;
	static Pb::ConnExtReqRelay reqRelay;
};
