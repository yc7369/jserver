#pragma once

#include<string>

struct ConnAuthSig  // �û���¼ʱconn_svr ���������֤��sig
{
	unsigned long long time_stamp;	//time(NULL)
	int plat_type; 
	int phone_type;	//ios or android
	unsigned int uin;	//ȫ���˺�
	char plat_acc_id[128];	//ƽ̨�˺�
	char user_name[128]; //�ǳ�
};