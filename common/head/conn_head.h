#pragma once

#include "base_type.h"
#include "protobuf_head.h"

struct ConnMsgHead {
	uint32 len;
	uint32 cmd_id;
	uint64 game_id;
	uint32 uin;
	uint32 body_len;
};

struct ConnSvrMsg
{
	ConnMsgHead *head;
	google::protobuf::Message *body;
};