/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：main.cc
*   创 建 者：Jeson Yang
*   创建日期：2018年07月29日
*   描    述：
*
================================================================*/
#include "stdafx.h"

#include "world.h"
#include "zone_define.h"

bool g_zone_running;
int g_imon_tid;

bool ParseArgs(int argc, char *argv[], CommandLineParams &cmd_param)
{
	if(argc <= 1)
	{
		printf("usage: %s -v\n", argv[0]);
		printf("usage: %s configfile path [-nd]\n", argv[0]);	

		return false;
	}

	if(0 == stricmp(argv[1], "-v"))
	{
		printf("server version %s, build in %s %s", SERVER_VERSION, __DATE__, __TIME__);
		return false;
	}

	strcpy(cmd_param.config_file, argv[1]);

	cmd_param.is_daemon = true;	//默认后台进程
	cmd_param.is_resume = false;	//共享内存还原重启


	for(int i = 0; i < argc; ++i)
	{
		if(0 == stricmp(argv[i], "-nd"))
		{
			cmd_param.is_daemon = false;	
			break;
		}		

	}

	return true;
}

void SigExitAfterAllKick(int sig_val)
{
	//踢出所有用户,回写缓存数据后退出,用户长时间运营维护, NORMAL重启前
	//
}

void ProcessSignal()
{
	signal(SIGHUP, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);

#if 0
	struct sigaction sig;
	sig.sa_handler = SIG_IGN;
	sig.sa_flags = 0;
	sigemptyset(&sig.sa_mask);
	sigaction(SIGPIPE, &sig, NULL);
#endif

	signal(SIGTERM, SigExitAfterAllKick);
	signal(SIGQUIT, SigExitAfterAllKick);

	//sigusr1,sigusr2 加载配置or游戏脚本?
//	signal(SIGUSR1, SigExitAfterAllKick);
}

int ZoneInit(int argc, char* argv[], bool &is_resume)
{
	CommandLineParams cmd_param;
	if(!ParseArgs(argc, argv, cmd_param))
	{
		return -1;
	}

	if(CWorld::Instance().InitConfig(cmd_param.config_file) < 0)
	{
		return -1;
	}

	if(CWorld::Instance().InitLog() < 0)
	{
		return -1;
	}

//	is_resume = IsZoneResume(g_svr_conf.pre_alloct_shm_key);
	cmd_param.is_resume = is_resume;

	srand(time(NULL));
	srandom(time(NULL));

	ProcessSignal();
	
	return 0;	
}

int ZonePrepare(bool is_resume)
{
    printf("\n\n====== zonesvrd begin to start, %s mode ======\n", is_resume ? "Resume" : "Normal");

    printf("zonesvrd prepare start ...\n");

    if(CWorld::Instance().Prepare(is_resume) < 0)
    {
        printf("zonesvrd prepare failed,exit!");
        return -1;
    }

    return 0;   
}


void MainLoop(bool isResume)
{
    printf("====== zonesvrd start success, %s mode ======\n", isResume ? "Resume" : "Normal");
    
    CWorld::Instance().Loop();
    
    return;
}

int main(int argc, char **argv)
{
    //InitUnhandledExceptionFilter();
    bool is_resume = false;
    g_zone_running = false;

    if(ZoneInit(argc,argv,is_resume) != 0)
    {
        return -1;
    }
 
    if(ZonePrepare(is_resume) != 0)
    {
        return -1;
    }

    g_zone_running = true;

    MainLoop(is_resume);

//    ZoneExit();

    return 0;
}
