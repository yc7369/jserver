/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：stdafx.h
*   创 建 者：Jeson Yang
*   创建日期：2018年07月29日
*   描    述：
*
================================================================*/


#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef WIN32 
#include <sys/time.h>
#include <limits.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#else /* WIN32 */

#ifndef _WIN32_WINNT        // Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501    // Change this to the appropriate value to target other versions of Windows.
#endif    

#define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers

#include "common.h"

#include <time.h>
#include "base_type.h"
#include "os_comm.h"
#include "os_type.h"
#define  hypotf _hypotf

#endif /* WIN32 */

#include <cassert>
#include <exception>
#include <cstddef>
#include <new>


#include <functional>
#include <algorithm>
#include <vector>
#include <math.h>
#include <errno.h>
#include <list>
#include <map>
#include <limits.h>
#include <bitset>
#include <utility>
#include <stdio.h>
#include <string.h>

#if 0
#include "obj_pool.h"
#include "obj_desc.h"
#include "obj_type.h"
#include "log_wrap.h"
#include "xxz_tlog.h"
#include "timer_queue.h"
#endif
#include <sys/stat.h>
#include <sys/shm.h>
#include <math.h>
#if 0
#include "protobuf_type.h"
#include "protobuf_message_pool.h"
#include "player_db_data.h"
#include "zone_common.h"
#endif
#ifndef PATH_MAX
#define PATH_MAX 512 // to make sure marco PATH_MAX defined in limits.h
#if 0
extern long long atoll(const char *nptr);
extern int strcasecmp(const char *s1, const char *s2);
extern int strncasecmp(const char *s1, const char *s2, size_t n);
#endif

#endif /* PATH_MAX */

#ifdef WIN32

#else
	#define stricmp strcasecmp
	#define strnicmp strncasecmp
#endif


#ifndef element_count
#define element_count(x) (sizeof((x)) / sizeof((x)[0]))
#endif

/* __STDAFX_H__ */


