/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：channel_def.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月22日
*	 描述:		
*
================================================================*/


#pragma once

enum
{
	CHANNEL_KEY_C2G = 0x1000,	//conn->game
	CHANNEL_KEY_G2C = 0x1001,	//game->conn


};

enum
{
	CHANNEL_SIZE_C2G = 1024 * 1024 * 100,	//conn->game 100M
	CHANNEL_SIZE_G2C = 1024 * 1024 * 100,	//game->conn 100M

};

enum 
{
	MAX_CLIENT_PACK_LEN = 1024 * 400,
	MAX_SERVER_PACK_LEN = 1024 * 1024,
};
