/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：os_comm.cc
*   创 建 者：Jeson Yang
*   创建日期：2018年08月05日
*	 描述:		
*
================================================================*/


#include "os_comm.h"
#include "common.h"

#ifdef WIN32
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#include <limits.h>
#include <stdint.h>
#include <strings.h>

#include <sys/types.h>   
#include <dirent.h>
#include <signal.h>
#endif


void SleepMS(unsigned int ms)
{
#ifdef WIN32
    Sleep(ms);
#else
    uint64 us = ms * 1000;
    usleep(us);
#endif
}

void SleepUS(unsigned long long us)
{
#ifdef WIN32
    unsigned long long  ms = us / 1000;
    Sleep((unsigned int)ms);
#else
    usleep(us);
#endif
}

/*
static void Terminate(int )
{
    g_runFlag.SetFlag(FLG_QUIT_NORMAL);
}
*/

/*
static void TermProcSignal(int sign)
{
struct sigaction sig;

sig.sa_handler = Terminate;
sig.sa_flags = 0;
sigemptyset(&sig.sa_mask);
sigaction(sign, &sig, NULL);
}
*/


void IgnorePipe()
{
    struct sigaction sig;

    sig.sa_handler = SIG_IGN;
    sig.sa_flags = 0;
    sigemptyset(&sig.sa_mask);
    sigaction(SIGPIPE,&sig,NULL);
}

void IgnoreSignal(int sign)
{

    struct sigaction sig;

    sig.sa_handler = SIG_IGN;
    sig.sa_flags = 0;
    sigemptyset(&sig.sa_mask);
    sigaction(sign, &sig, NULL);
}

//后台方式启动
void InitDaemon()
{
    pid_t pid;

    if ((pid = fork()) != 0)
        exit(0);

    setsid(); // Become session leader

    if ((pid = fork()) != 0)
        exit(0);

    umask(0); // clear our file mode creation mask
}

bool createDir(const char *dirPath)
{
    std::string path = dirPath;

    std::string::size_type pos = path.find_first_of("/\\", 0);
    while (pos != std::string::npos)
    {
        std::string subPath = path.substr(0, pos + 1);

        int ret = access(subPath.c_str(), 0);
        if (ret != 0)
        {
            ret = mkdir(subPath.c_str(), 0777);
            if (ret != 0)
            {
                return false;
            }
        }
        pos = path.find_first_of("/\\", pos + 1);
    }
    return true;
}

