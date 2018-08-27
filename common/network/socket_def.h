/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：socket_def.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月07日
*	 描述:		
*
================================================================*/


#pragma once

// ----------------------------------------
// common unix includes / defines
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdio.h>
#define Errno errno
#define StrError strerror

// WIN32 adapt
#define closesocket close
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
typedef int HSOCKET;

#ifndef INADDR_NONE
#define INADDR_NONE ((unsigned long) -1)
#endif

#ifdef WIN32
#else
#include <netinet/tcp.h>
#include <errno.h>
#endif

typedef unsigned long ipaddr_t;
typedef unsigned short port_t;

typedef int hconn_t;

typedef long long int64;
typedef unsigned long long uint64;

#define INVALID_HCONN (-1)
