/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：stdafx.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月05日
*	 描述:		
*
================================================================*/

#ifndef STDAFX_HEADER_
#define STDAFX_HEADER_
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						
#ifdef WIN32

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif		
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <WinSock2.h>
#include <sys/timeb.h>
#include <atlconv.h>
#include <direct.h>

#else
#include <sys/time.h>
#include <sys/file.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <arpa/inet.h>
#endif /* WIN32 */

#include <stdio.h>
#include <string.h>
#include <exception>
#include <new>
#include <cstddef>
#include <algorithm>
#include <time.h>
#include <iterator>
#include <cassert>
#include <list>
#include <sys/procfs.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <sys/mman.h>

//#include "platform_base.h"

#endif

