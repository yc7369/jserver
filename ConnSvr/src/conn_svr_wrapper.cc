#include "conn_svr_wrapper.h"

Pb::ConnExtStart CConnSvrWrapper::start;
Pb::ConnExtStop CConnSvrWrapper::stop;
Pb::ConnExtBroadcast CConnSvrWrapper::broadcast;
Pb::ConnExtReconn CConnSvrWrapper::reconn;
Pb::ConnExtReqRelay CConnSvrWrapper::reqRelay;
Pb::ConnExtNotifyRelay CConnSvrWrapper::notifyRelay;

CConnSvrWrapper::CConnSvrWrapper()
{
}

CConnSvrWrapper::~CConnSvrWrapper()
{
}

int CConnSvrWrapper::Init()
{
	return 0;
}

int CConnSvrWrapper::EncodeConnSvrHead(char* buff, int *buff_len, char cmd, uint64 game_id, int cmd_len, int reason)
{
	msg.head = (ConnMsgHead*)buff;
	msg.head->cmd_id = cmd;
	msg.body = NULL;

	switch (cmd)
	{
	case Pb::CONN_CMD_START:
	{
		start.Clear();
		break;
	}
	
	case Pb::CONN_CMD_STOP:
	{
		break;
	}
	
	case Pb::CONN_CMD_NORMAL:
	{
		break;
	}

	case Pb::CONN_CMD_RECONN:
	{
		break;
	}

	default:
		return -1;
	}
	return 0;
}

int CConnSvrWrapper::DecodeConnSvrHead(char *buff, int buff_len, int &head_len)
{
	return 0;
}