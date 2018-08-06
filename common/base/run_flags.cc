/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：run_flags.cc
*   创 建 者：Jeson Yang
*   创建日期：2018年08月04日
*   描    述：
*
================================================================*/


#include "run_flags.h"

CRunFlag::CRunFlag()
{
	flag_ = 0;
}

CRunFlag::~CRunFlag()
{
}

CRunFlag::FlagType CRunFlag::GetAllFlag()
{
	return flag_;
}

int CRunFlag::ClearFlag(FlagType flag)
{
	if(flag_ <= (0x1 <<7))
	{
		//start or quit 
		return 0;
	}

	FlagType temp;
	temp = flag ^ 0xffff;
	flag_ &= temp;
	return 0;
}

int CRunFlag::SetFlag( FlagType flag )
{
    if ( flag <= (0x1 << 3))
    {
    	//start mode
        flag_ = flag_ & 0xfff0;
        flag_ = flag_ | flag;            
    }
    else if (flag <= (0x1 << 7))
    {
        //quit mode
        flag_ = flag_ & 0xff0f;
        flag_ = flag_ | flag;            
    }
    else
    {
	    flag_ = flag_ | flag;
	}
	return 0;
}


bool CRunFlag::IsFlagSet(FlagType flag)
{
	if( flag_ & flag )
	{
		return true;
	}
	else
	{
		return false;
	}
}
