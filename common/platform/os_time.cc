/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：os_time.cc
*   创 建 者：Jeson Yang
*   创建日期：2018年08月05日
*	 描述:		
*
================================================================*/

#include "os_time.h"
#include "os_comm.h"


char *GetTimeFormatStr(char *dateTime, const struct timeval &tv, TimeFormat fmt /* = TF_DATE_TIME_SEC */)
{
	struct tm ltm;
	time_t tmSec = tv.tv_sec;
	localtime_r(&tmSec, &ltm);

	switch (fmt)
	{
	default:
		{
			dateTime[0] = '\0';
		}
		break;
	case TF_DATE:
		{
			sprintf(dateTime, "%04d-%02d-%02d",
				ltm.tm_year + 1900, ltm.tm_mon + 1, ltm.tm_mday);		
		}
		break;
	case TF_DATE_PACKED:
		{
			sprintf(dateTime, "%04d%02d%02d",
				ltm.tm_year + 1900, ltm.tm_mon + 1, ltm.tm_mday);
		}
		break;
	case TF_DATE_TIME_SEC:
		{
			sprintf(dateTime, "%04d-%02d-%02d %02d:%02d:%02d",
				ltm.tm_year + 1900, ltm.tm_mon + 1, ltm.tm_mday,
				ltm.tm_hour, ltm.tm_min, ltm.tm_sec);
		}
		break;
	case TF_DATE_TIME_SEC_PACKED:
		{
			sprintf(dateTime, "%04d%02d%02d%02d%02d%02d",
				ltm.tm_year + 1900, ltm.tm_mon + 1, ltm.tm_mday,
				ltm.tm_hour, ltm.tm_min, ltm.tm_sec);
		}
		break;
	case TF_DATE_TIME_MS:
		{
			sprintf(dateTime, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
				ltm.tm_year + 1900, ltm.tm_mon + 1, ltm.tm_mday,
				ltm.tm_hour, ltm.tm_min, ltm.tm_sec, int(tv.tv_usec / 1000));
		}
		break;
	case TF_DATE_TIME_MS_PACKED:
		{
			sprintf(dateTime, "%04d%02d%02d%02d%02d%02d%03d",
				ltm.tm_year + 1900, ltm.tm_mon + 1, ltm.tm_mday,
				ltm.tm_hour, ltm.tm_min, ltm.tm_sec, int(tv.tv_usec / 1000));
		}
		break;
	}

	return dateTime;
}

