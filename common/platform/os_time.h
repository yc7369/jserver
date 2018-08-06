/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：os_time.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月05日
*	 描述:		
*
================================================================*/
#pragma once

#ifdef WIN32
#	include "windows.h"
#	pragma comment(lib, "kernel32.lib")
#else
#endif


#ifdef WIN32
#   include <time.h>
#	include <sys/timeb.h>
#else
#	include <time.h>
#	include <sys/time.h>
#   include <sys/timeb.h>
#endif

#include "os_type.h"

#ifdef WIN32
#pragma comment(lib, "winmm.lib")
#endif

enum TimeConstants
{
	MINUTE = 60,
	HOUR   = MINUTE*60,
	DAY    = HOUR*24,
	MONTH  = DAY*30
};

inline struct timeval GetTimeVal()
{
	struct timeval tv;

#ifdef WIN32

	struct timeb tp;
	ftime(&tp);

	tv.tv_sec = (long)tp.time;
	tv.tv_usec = tp.millitm * 1000;

#else 

	gettimeofday(&tv, 0);

#endif

	return tv;
}



inline time_t GetTimeSec()
{
	return (time_t)time(NULL);
}

inline int64 GetTimeMs()
{
/*	uint32 ms = 0;
 *
 *		struct timeb tp;
 *			ftime(&tp);
 *
 *				ms = (uint32)(tp.time * 1000 + tp.millitm);
 *				*/
	timeval tv = GetTimeVal();
	int64 ms = (int64)tv.tv_sec * 1000 + tv.tv_usec / 1000;

	return ms;
}

inline uint64 GetTimeUs()
{
	uint64 time_in_us = 0;

#ifdef WIN32
	time_in_us = 1000 * GetTimeMs();
#else 
	struct timeval tv;
	gettimeofday(&tv, 0);
	time_in_us = (uint64)tv.tv_sec * 1000000 + tv.tv_usec;
#endif

	return time_in_us;
}

#ifdef WIN32
inline struct tm* mylocaltime_r(const time_t *t, struct tm* stm)
{
	struct tm *lt = localtime(t);
	*stm = *lt;
	return stm;
}
#	ifndef localtime_r
#		define localtime_r mylocaltime_r
#	endif

#endif

class TimeStats
{
public:
	TimeStats()
		: t1_(0)
		, t2_(0)
	{
	}
	void Begin()
	{
		t1_ = GetTimeUs();
	}
	void End()
	{
		t2_ = GetTimeUs();
	}
	uint64 GetExpireTime64()
	{
		return t2_ - t1_;
	}
	uint32 GetExpireTime32()
	{
		return (uint32)(t2_ - t1_);
	}
private:
	uint64 t1_;
	uint64 t2_;
};

class TimeStatHelper
{
	TimeStatHelper()
	{
		ts_.Begin();
	}
	
	uint32 operator()()
	{
		ts_.End();
		return ts_.GetExpireTime32();
	}
private:
	TimeStats ts_;
};

enum TimeFormat
{
	TF_DATE = 1,
	TF_DATE_PACKED = 2,
	TF_DATE_TIME_SEC = 3,
	TF_DATE_TIME_SEC_PACKED = 4,
	TF_DATE_TIME_MS = 5,
	TF_DATE_TIME_MS_PACKED = 6,
};

char *GetTimeFormatStr(char *dateTime, const struct timeval &tv, TimeFormat fmt = TF_DATE_TIME_SEC);


class TimeStr
{
public:
	TimeStr()
	{
		buf_[0] = '\0';
	}
	const char *operator()(time_t t)
	{
		timeval tv;
		tv.tv_sec = (long)t;
		tv.tv_usec = 0;

		GetTimeFormatStr(buf_, tv, TF_DATE_TIME_SEC);

		return buf_;
	}
	const char *operator()(const timeval &tv)
	{
		GetTimeFormatStr(buf_, tv, TF_DATE_TIME_MS);

		return buf_;
	}
private:
	char buf_[100];
};


