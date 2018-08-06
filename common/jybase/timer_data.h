/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：timer_data.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月05日
*	 描述:		
*
================================================================*/


#pragma once

#include "obj_desc.h"
namespace ObjManager
{
    template <class T>
    struct tagDataHelper;
    template <class T>
    struct tagAgentHelper;

    template <class DataType>
    typename tagAgentHelper<DataType>::AgentType* GetAgent(DataType* first);
    template <class DataType>
    const typename tagAgentHelper<DataType>::AgentType* GetAgent(const DataType* first);
}

namespace jy 
{

	// 所有的定时器Node均最终需要从ITimerTask派生
	// 任何从ITimerTask派生，其长度不能超过tagTimerNode::MAX_OBJ_LECNGTH
	// 所有从ITimerTask派生的类必须提供拷贝构造函数(用于创建对象)以及
	// 默认的构造函数(用于服务器重新启动的时候初始化信息。一般不做任何操作)
	// 即便不做任何操作，也要声明此函数；否则vc下面debug表现不正常，因为vc在
	// debug默认是初始化为0
	class ITimerTask
	{
	public:
		virtual void run() = 0;
		virtual ~ITimerTask()
		{
		}
		virtual unsigned int type() const = 0;
	};


	template <class ObjType>
	struct tagObjectIDHelper;

	// 暂时只支持代理对象设置定时器
	template <class ObjType>
	struct tagObjectIDHelper
	{
		typedef ObjType ObjectType;
		typedef IDType ObjectIDType;
		inline ObjectType* operator()()
		{
            typedef typename ObjManager::tagDataHelper<ObjectType> DataHelper;
			typedef typename DataHelper::DataType DataType;
			DataType* data =  m_objectID.GetObject<DataType>();
			if (data)
			{
				return ObjManager::GetAgent(data);
			}
			else
			{
				return 0;
			}
		}

		tagObjectIDHelper()
		{
		}

		tagObjectIDHelper(const ObjectIDType& id)
			: m_objectID(id)
		{
		}
		ObjectIDType m_objectID;
	};	

	template <class IDHelper, class TimerTaskDataType>
	struct tagTimerDataHelper
	{
		typedef typename IDHelper::ObjectType ObjectType;
		typedef typename IDHelper::ObjectIDType ObjectIDType;
		tagTimerDataHelper(const ObjectIDType& objectID, const TimerTaskDataType& data)
			: m_objectID(objectID), m_data(data)
		{
		}

		tagTimerDataHelper(const tagTimerDataHelper& task)
			: m_objectID(task.m_objectID), m_data(task.m_data)
		{
		}

		tagTimerDataHelper()
		{
		}

		template <class FunctionType>
		void invoke(FunctionType func)
		{
			ObjectType* obj = m_objectID();
			if (obj)
			{
				func(obj, m_data);
			}
		}
		
		IDHelper m_objectID;
		TimerTaskDataType m_data;
	};

	// 定时器超时不用任何输入参数
	template <class IDHelper>
	struct tagTimerDataHelper <IDHelper, void>
	{
		typedef typename IDHelper::ObjectType ObjectType;
		typedef typename IDHelper::ObjectIDType ObjectIDType;

		tagTimerDataHelper(const ObjectIDType& objectID)
			: m_helper(objectID)
		{
		}

		tagTimerDataHelper(const tagTimerDataHelper& task)
			: m_helper(task.m_helper)
		{
		}

		tagTimerDataHelper()
		{
		}

		template <class FunctionType>
		void invoke(FunctionType func)
		{
			ObjectType* obj = m_helper();
			if (obj)
			{
				func(obj);
			}
		}

		IDHelper m_helper;
	};

	template <class IDHelper, class FunctionType, class TimerTaskDataType = void>
	struct TTimerTask : public ITimerTask
	{
		typedef tagTimerDataHelper<IDHelper, TimerTaskDataType> HelperType;

		TTimerTask(const HelperType& helper)
			: m_helper(helper)
		{
		}

		TTimerTask(const TTimerTask& task)
			: m_helper(task.m_helper)
		{
		}

		TTimerTask()
		{
		}

		virtual void run()
		{
			m_helper.invoke(FunctionType());
		}

		virtual unsigned int type() const
		{
			return FunctionType().type();
		}
		HelperType m_helper;
	};

	template <class T>
	struct TimerConstruct
	{
		void operator()(char* buffer, const ITimerTask* task)
		{
			assert(sizeof(T) <= TIMER_DATA_LEN);
			if (task)
			{
				const T* srcTask = reinterpret_cast<const T*>(task);
				new (buffer) T(*srcTask);
			}
			else
			{
				new (buffer) T;
			}
		}
	};

	inline void RunTask(char* buffer)
	{
		ITimerTask* task = reinterpret_cast<ITimerTask*>(buffer);
		task->run();
	}
	extern void ConstructTask(char* buffer, const ITimerTask* task, unsigned int type);
}
