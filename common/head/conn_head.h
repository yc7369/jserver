#pragma once

#include "base_type.h"

struct ConnMsgHead {
	uint32 len;
	uint32 cmd_id;
	uint64 game_id;
	uint32 uin;
	uint32 body_len;
};