/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：os_comm.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月05日
*	 描述:		
*
================================================================*/


#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <vector>
#include <stdlib.h>

#ifdef WIN32

#define I64FMT "%016I64X"
#define I64FMTD "%I64u"
#define SI64FMTD "%I64d"
#define snprintf _snprintf
#define atoll __atoi64
#define vsnprintf _vsnprintf
#define strdup _strdup

#define strcasecmp stricmp
#define strncasecmp  strnicmp

#else

#define stricmp strcasecmp
#define strnicmp strncasecmp
#define I64FMT "%016llX"
#define I64FMTD "%llu"
#define SI64FMTD "%lld"

#define _atoi64(x) strtoll((x), NULL, 10)
#define _atoui64(x) strtoull((x), NULL, 10)

inline char* _i64toa(long long num, char* buffer, int base = 10)
{
    sprintf(buffer, "%lld", num);
    return buffer;
}

inline char* _ui64toa(unsigned long long num, char* buffer, int base = 10)
{
    sprintf(buffer, "%llu", num);
    return buffer;
}

#endif

#include "os_type.h"


#ifdef WIN32
#define PATH_MAX _MAX_PATH
#define snprintf _snprintf
#include <io.h>
#else
//linux
#include <unistd.h>
#include <limits.h>
#include <stdint.h>
#include <strings.h>
#include <fcntl.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#endif

#include <string>

using namespace std;

void SleepMS(unsigned int ms);
void SleepUS(unsigned long long us);

void IgnorePipe();
void IgnoreSignal(int sign);
void InitDaemon();

bool createDir(const char *dirPath);

#ifdef WIN32
inline void srandom(unsigned int seed)
{
    srand(seed);
}

inline int random()
{
    return rand();
}

#endif




