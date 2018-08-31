#pragma once

#include<string>

struct ConnAuthSig  // 用户登录时conn_svr 进行身份验证的sig
{
	unsigned long long time_stamp;	//time(NULL)
	int plat_type; 
	int phone_type;	//ios or android
	unsigned int uin;	//全局账号
	char plat_acc_id[128];	//平台账号
	char user_name[128]; //昵称
};