/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：base_exception.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月05日
*	 描述:		
*
================================================================*/


#pragma once


#include <exception>
#include <string>

class base_exception : public std::exception
{
public:
	base_exception(const std::string& exception_desc) throw ()
	: desc(exception_desc)
	{
	}

    virtual ~base_exception() throw()
	{
	}

	virtual const char * what() const throw ()
	{
		return desc.c_str();
	}
private:
	std::string desc;
};

