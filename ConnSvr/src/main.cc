/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：main.cc
*   创 建 者：Jeson Yang
*   创建日期：2018年08月19日
*	 描述:		
*
================================================================*/

#include "stdafx.h"
#include "conn_svr.h"
#include "os_comm.h"

bool ParseArgs(int argc, char *argv[], CommandLineParams &cmd_param)
{
    if ( argc < 2 )
    {
        return false;
    }
    if (2 == argc && 0 == stricmp(argv[1], "-v"))
    {
        printf("server version %s, build in %s %s", SERVER_VERSION, __DATE__, __TIME__);
        return false;
    }
    strncpy(cmd_param.config_file, argv[1], sizeof(cmd_param.config_file));

    cmd_param.daemon = true;
    for (int i = 0; i < argc; ++i)
    {
        if (0 == stricmp(argv[i], "-nd"))
        {
            cmd_param.daemon = false;
            break;
        }
    }
    return true;
}

void ProcessSignal()
{   
    IgnoreSignal(SIGHUP);
    IgnoreSignal(SIGPIPE);
    IgnoreSignal(SIGTTOU);
    IgnoreSignal(SIGTTIN);
    IgnoreSignal(SIGCHLD);
    IgnoreSignal(SIGINT);
}

int main(int argc, char *argv[])
{
	CommandLineParams cmd_param;
	if(ParseArgs(argc, argv, cmd_param))
	{
		return -1;
	}

	if(cmd_param.daemon)
	{
		InitDaemon();
		ProcessSignal();
	}

	ConnServer svr;
	if(!svr.Init(cmd_param))	
	{
		return -1;
	}	

	svr.Run();

	return 0;
}
