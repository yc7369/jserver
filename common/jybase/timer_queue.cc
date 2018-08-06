/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：timer_queue.cc
*   创 建 者：Jeson Yang
*   创建日期：2018年08月05日
*	 描述:		
*
================================================================*/


#include "timer_queue.h"
#include "timer_data.h"
#include "sys_time.h"
#include "base_exception.h"


static int s_timerTraceFlag = 0;

namespace jy
{
    long g_systemStart = 0;
#   ifdef WIN32
    int gettimeofday(struct timeval *tv, struct timezone *)
    {
        struct _timeb stTime;

        if( !tv )
        {
            return -1;
        }

        _ftime( &stTime );
        tv->tv_sec = (long)stTime.time;
        tv->tv_usec = 1000 * stTime.millitm;

        return 0;
    }   
#   endif /* WIN32 */   
    
	time_t GetSystemStartTime()
	{
		return g_systemStart;
	}
    namespace framework
    {
        static unsigned int s_currentTick = 0;
        static timeval s_currentTimeVal ;
        static int s_time_increment = 0;    //时间增量, 以实现GM调整时间
        
        // 如果提高精度，需要对GetCurrentTick做处理，以防止整数越界
        uint32 GetCurrentTick()
        {
            return s_currentTick;
        }

        //返回从zone启动到当前经历的毫秒数, 注意int类型只可表示24天的时间范围
        int GetCurrentMs()
        {
            return (int)( (s_currentTimeVal.tv_sec - g_systemStart) * 1000 
                + s_currentTimeVal.tv_usec / 1000 ); //加10天的毫秒数
        }

        time_t GetCurrentTimeVal(struct timeval * tv)
        {
            if (tv != NULL)
            {
                *tv = s_currentTimeVal;
            }
            return s_currentTimeVal.tv_sec;
        }
        
        time_t GetCurrentTime()
        {
            return s_currentTimeVal.tv_sec;
        }

        time_t GetCurrentTimeUsec(struct timeval *tv)
        {
            if (tv != NULL)
            {
                *tv = s_currentTimeVal;
            }
            return s_currentTimeVal.tv_usec;
        }

        time_t TickCurrentTimeVal(struct timeval *tv)
        {
            UpdateCurrTime();
            return GetCurrentTimeVal(tv);
        }
        
        void UpdateCurrTime()
        {
            gettimeofday(&s_currentTimeVal, NULL);
            s_currentTimeVal.tv_sec += s_time_increment;    //加上时间增量, 用于实现GM调整时间
        }
        
        void SetTimeIncrement(int addSec)   //设置时间增量, 用于实现GM调整时间
        {
            s_time_increment = addSec;
        }
        
        int GetTimeIncrement()  //获取时间增量
        {
            return s_time_increment;
        }
        
    }
    


    void SetTraceFlag() 
    { 
        s_timerTraceFlag = 1; 
    }
    
    void ClrTraceFlag() 
    { 
        s_timerTraceFlag = 0; 
    }

    inline void Resume(tagTimerNode& node)
    {
        ConstructTask(node.m_taskData, 0, node.m_timerType);
    }

    uint64 CTimerQueue::Schedule(const ITimerTask& task, uint32 delay, uint32 period, uint32 repeat_count /* = 1 */)
    {
        unsigned int type = task.type();
        if (m_freeList.empty())
        {
            //WRITE_ERR_LOG("No more timer space for timer task, type is %d", type);
            return C_INVALID_TIMER_ID;
        }

		if ( (0 == period) && ( repeat_count > 1))
		{
			//WRITE_ERR_LOG("Invalid timer schedule. type:%u, period:%u, repeat_count:%u", type, period, repeat_count);
			return C_INVALID_TIMER_ID;			
		}
        
        if ( delay == 0 )
        {
            //WRITE_ERR_LOG("Timer schedule param fixed, delay 0 to 1, type:%u", type);
            delay = 1;
        }
        
        NodeType& node = m_freeList.back();
        m_freeList.pop_back(); // 要立即调用pop_back，因为pop_back需要使用node的数据，而这些数据下面可能修改

        tagTimerNode& timerNode = node.m_data;
        ConstructTask(timerNode.m_taskData, &task, type);
        timerNode.m_expire = framework::GetCurrentTick() + delay;
        timerNode.m_period = period;
        timerNode.m_repeatCount = repeat_count;
        timerNode.m_timerType = type;
        StepUpTimerSeq(timerNode.m_timerSeq);

        m_bucket[HashTick(timerNode.m_expire)].push_back(&node);
        uint32 id = uint32((&node - m_nodeBase) + 1); // base on one
        uint64 timerid =  EncodeTimerId(timerNode.m_timerType, id, timerNode.m_timerSeq);
        assert(timerid != C_INVALID_TIMER_ID);

        //加入了新的timer，统计
        AddTimerCount(type);


        //     WRITE_DBG_LOG("CTimerQueue::Schedule timerid:%llu, type:%u, pos:%u, seq:%u, (%u,%u,%u)",  timerid, type, id, timerNode.m_timerSeq, delay, period, repeat_count);
        return timerid;
    }

    void CTimerQueue::Cancel(uint64& timerid)
    {
        if (C_INVALID_TIMER_ID != timerid)
        {
            uint32 id = 0;
            unsigned short seq = 0;
            unsigned short type = 0;
            if (DecodeTimerId(timerid, type,  id , seq) != 0)
            {
                //WRITE_ERR_LOG("invalid timerid :"FMT_UINT64", type:%u, pos:%u, seq:%u", timerid, type, id, seq);
                return;
            }

            //assert((id <= m_count) && (id > 0));
            --id; // base on one
            if (id >= m_count)
            {
				  //WRITE_ERR_LOG("invalid timerid :"FMT_UINT64", type:%u, pos:%u, seq:%u", timerid, type, id, seq);
				  return;            
            }

			 tagTimerNode& delNode = m_nodeBase[id].m_data;
            
            if ((seq != delNode.m_timerSeq)  || (type != delNode.m_timerType))
            {
                //流水号不一致, 说明持有的是旧ID.
                //WRITE_ERR_LOG("invalid cancel timer. timerid:"FMT_UINT64", input type:%u, seq:%u, node type:%u, node seq:%u ", 
                //    timerid, type, seq, 
                //    delNode.m_timerType, delNode.m_timerSeq);
                //assert(false);
                return;
            }

            if (m_nodeBase[id].IsOrphanNode())
            {
                // 目前该定时器正在被处理，因此把实际删除放在定时器处理完成之后来处理
                delNode.m_repeatCount = 0;
            }
            else
            {
                // assert(! m_freeList.is_in_stack(m_nodeBase[id]));
                if (! m_freeList.is_in_stack(m_nodeBase[id]))
                {
                    ArrayListType::iterator itr (m_nodeBase + id);
                    
                    if (m_tickIterator == itr)
                    {
                    		if ( delNode.m_expire != m_currentTick )
                    		{
                    			//WRITE_WARN_LOG("Timer Cancel , node not expire but is  m_tickIterator, Id "FMT_UINT64",  data:%u, %u, %u, %u,  %u;  m_currentTick:%u", 
                    			//	timerid, 
                    			//	delNode.m_timerType, delNode.m_repeatCount, delNode.m_expire, delNode.m_period, delNode.m_timerSeq,
								//	m_currentTick);
                    		}
                        // 需要判断是否是下一个iterator，如果是的话需要特别处理,修改m_tickIterator
                        m_tickIterator = m_bucket[HashTick(delNode.m_expire)].erase(itr);
                    }
                    else
                    {
                        m_bucket[HashTick(delNode.m_expire)].erase(itr);
                    }

                    // 只对没有删除的定时器做删除操作
                    ITimerTask* task = reinterpret_cast<ITimerTask*>(m_nodeBase[id].m_data.m_taskData);
                    task->~ITimerTask();
                    m_freeList.push_back(m_nodeBase[id]);
                    //timer统计
                    DecTimerCount(type);
                }
                else
                {
                    //WRITE_ERR_LOG("error! release a timer node which is in freelist. timerId:"FMT_UINT64", ID = %u", timerid, id);
                    return;
                }
            }
                    
                   //WRITE_DBG_LOG("CTimerQueue::Cancel timerid:%llu, type:%u, pos:%u, seq:%u", timerid, type, id+1, seq);
            timerid = C_INVALID_TIMER_ID;
        }
    }

    int CTimerQueue::Tick()
    {
        jy::framework::UpdateCurrTime();
		timeval &tv= jy::framework::s_currentTimeVal;

        // 这里做一个判断然后再设置是为了防止系统调整时间
        // 对于系统调整时间，如果向前调整(即增加时间，则快速跳过众多tick);反之，则等待一段时间再变化tick
        // 由于时间的错乱会引发表现的一些异常，因此在调整时间时，需要减少调整量，一般应控制在1秒以内
        uint32 currTick = uint32((tv.tv_sec - m_systemStart) * C_TICK_COUNT_PER_SECOND 
                    + tv.tv_usec / (ONE_SECOND_IN_USECS / C_TICK_COUNT_PER_SECOND));
        int count = 0;
        if (currTick > m_currentTick)
        {
            for (; m_currentTick < currTick; ++m_currentTick)
            {
                // framework::s_currentTick每次都设置，就是为了tick调用定时函数时，获得的当前tick等于请求tick预计发生的时间
                // 这样对性能只有及其微量的影响，但是增加了对时间管理的理解的便捷性
                framework::s_currentTick = m_currentTick;
                count += CheckTickArray(m_currentTick, HashTick(m_currentTick));
            }
        }
        return count;
    }
    
    //配合GM调时间, 向前跳过若干Tick, glory
    int CTimerQueue::SkipTick()
    {
        jy::framework::UpdateCurrTime();
        timeval &tv = jy::framework::s_currentTimeVal;
        
        uint32 currTick = uint32((tv.tv_sec - m_systemStart) * C_TICK_COUNT_PER_SECOND 
                    + tv.tv_usec / (ONE_SECOND_IN_USECS / C_TICK_COUNT_PER_SECOND));

        //先将s_currentTick跳变到未来时间点, 即GM设置的未来时间点
        jy::framework::s_currentTick = currTick;
        
        int count = 0;
        if (currTick > m_currentTick)
        {
            for (; m_currentTick < currTick; ++m_currentTick)
            {
                //在此处理每个Tick时, 新产生的Timer按未来时间来放置Tick槽
                //这样可以避免调整时间后的长期CPU100, 同时需要注意这里的Tick跳变
                count += SkipTickArray(m_currentTick, HashTick(m_currentTick));
            }
        }
        return count;
    }
    

    bool CTimerQueue::IsValidNodePointer(NodeType * node)
	{
		char* ptrNode = (char*)node;

		char* ptrStart = (char*)m_nodeBase;
		char* ptrEnd = ptrStart + sizeof(NodeType)*m_count;

		if ((ptrNode < ptrStart) || (ptrNode > ptrEnd))
		{
			//WRITE_ERR_LOG("Invalid Timer Node Pointer! %p  [%p,%p]", ptrNode, ptrStart, ptrEnd);	
			return false;
		}

		int dlt = (ptrNode - ptrStart) % sizeof(NodeType);
		if ( 0 != dlt )
		{
			//WRITE_ERR_LOG("Invalid Timer Node Pointer! %p  [%p,%p], %d", ptrNode, ptrStart, ptrEnd, dlt);	
			return false;			
		}
		return true;
	}

	//TIME LIST出错处理
    int CTimerQueue::DoBadTimeList(int pos,  ArrayListType::iterator curItr,  bool doFix)
    {
		//WRITE_ERR_LOG("DoBadTimeList ! Iterator next is self. pos:%d", pos);

		//打印当前节点，及前面最多五个节点
		ArrayListType::iterator tmp_it = curItr;
		int tmpCount = 0;
		while (tmp_it  != m_bucket[pos].end())
		{
			NodeType* tmpNode = reinterpret_cast<NodeType*>(tmp_it.m_node);
			if (!IsValidNodePointer(tmpNode))
			{
				break;
			}
			//tagTimerNode& tmpTimerNode = tmpNode->m_data;
			
			//WRITE_ERR_LOG("DoBadTimeList fail! next:"FMT_SIZET", prev:"FMT_SIZET"",	tmpNode->m_next, tmpNode->m_prev);
			//WRITE_ERR_LOG("DoBadTimeList fail! timer_data:%u, %u, %u, %u, %u", 
			//tmpTimerNode.m_timerType, tmpTimerNode.m_repeatCount, tmpTimerNode.m_expire, 
			//tmpTimerNode.m_period, tmpTimerNode.m_timerSeq);
			
			--tmp_it; 	//往前追溯
			++tmpCount;
			if (tmpCount > 5)
			{
				break;
			}
		}

		if (NULL == m_runFlag)
		{
			//需要做退出处理，必须有runFlag
			assert(false);
		}

		if (doFix)
		{
			//WRITE_ERR_LOG("DoBadTimeList ! try fix. pos:%d", pos);
			tmpCount = 0;
			bool fixListOk = false;
			//试着恢复，因为prev链是OK的，以此链为基础恢复next链
			ArrayListType::iterator itr = m_bucket[pos].rbegin();
			ArrayListType::iterator next_itr = m_bucket[pos].end();
			
			while (itr != m_bucket[pos].end())
			{
				NodeType* tmpNode = reinterpret_cast<NodeType*>(itr.m_node);
				if (!IsValidNodePointer(tmpNode))
				{
					break;
				}
				if (itr == curItr)
				{
					//找到了错误的节点，重建next链
					m_bucket[pos].fix_next_link(curItr.m_node, next_itr.m_node);
					fixListOk = true;
					//WRITE_ERR_LOG("DoBadTimeList succ! ");
					break;
				}
				else
				{
					//继续做prev
					next_itr = itr;
					--itr;
					
					++tmpCount;
					if (tmpCount > 10000)
					{
						//最多只找10000个节点
						//WRITE_ERR_LOG("DoBadTimeList fail! too many node. %d", tmpCount);
						break;
					}
				}
			}

			if ((fixListOk) && (curItr != next_itr))
			{
				return 0;
			}
		}
		
		//出错处理
		if (m_runFlag->IsFlagSet(FLAG_QUIT_AFTER_KICK_ALL_USER))
		{
			//不重复设置退出
			//WRITE_ERR_LOG("DoBadTimeList fail! already set kill user");
		}
		else
		{
			m_runFlag->SetFlag( FLAG_QUIT_AFTER_KICK_ALL_USER);//以踢人方式退出
			m_runFlag->SetFlag( FLAG_QUIT_COREDUMP);//以coredump方式退出
			//WRITE_ERR_LOG("DoBadTimeList fail! KICK ALL USER!!!");
		}

		return -1;
    	
    }
    
    int CTimerQueue::CheckTickArray(uint32 tick, int pos)
    {
        int count = 0;
        m_tickIterator = m_bucket[pos].begin();
        for (; m_bucket[pos].end() != m_tickIterator;)
        {
            array_list_iterator_base& base = m_tickIterator;
            NodeType* node = reinterpret_cast<NodeType*>(base.m_node);
            tagTimerNode& timerNode = node->m_data;
            if (tick >= timerNode.m_expire)
            {
				  assert (!m_freeList.is_in_stack(*node));
                ++count;
                --timerNode.m_repeatCount;

                m_tickIterator = m_bucket[pos].erase(m_tickIterator);
                
                // 这样在超时处理函数中可以调用cancel取消本定时器
                RunTask(timerNode.m_taskData);
                
                //tlog_increase_eventid();

                // 在RunTask中可以调用Cancel函数改变timerNode.m_repeatCount，但不会真正调用析构函数和队列处理函数
                if (0 != timerNode.m_repeatCount)
                {
                    timerNode.m_expire += timerNode.m_period;
                    m_bucket[HashTick(timerNode.m_expire)].push_back(node);
                }
                else
                {
                                 //WRITE_DBG_LOG("CTimerQueue::CheckTickArray DelTimer type:%u, pos:%u, seq:%u", timerNode.m_timerType, uint32((node - m_nodeBase) + 1), timerNode.m_timerSeq);
                    //timer数量统计
                    DecTimerCount(timerNode.m_timerType);

                    assert (!m_freeList.is_in_stack(*node));
                    ITimerTask* task = reinterpret_cast<ITimerTask*>(timerNode.m_taskData);
                    task->~ITimerTask();
                    m_freeList.push_back(*node);
                }
            }
            else
            {
				 ArrayListType::iterator old_it = m_tickIterator;
				++m_tickIterator;
				if ((old_it == m_tickIterator) &&(m_bucket[pos].end() != m_tickIterator))
				{
					//节点的next还是自己，链表错误，需要做处理
					//WRITE_ERR_LOG("DoBadTimeList ! Iterator next is self. tick %u, %u", tick, timerNode.m_expire);
					 if (DoBadTimeList(pos, m_tickIterator) < 0)
					 {
						break;
					 }
				}
            }
        }
        return count;
    }
    
    //配合GM调时间, 向前跳过若干Tick, glory
    int CTimerQueue::SkipTickArray(uint32 tick, int pos)
    {
        int count = 0;
        uint32 currTick = jy::framework::GetCurrentTick();
        m_tickIterator = m_bucket[pos].begin();
        
        for (; m_bucket[pos].end() != m_tickIterator;)
        {
            array_list_iterator_base& base = m_tickIterator;
            NodeType* node = reinterpret_cast<NodeType*>(base.m_node);
            tagTimerNode& timerNode = node->m_data;
            if (tick >= timerNode.m_expire)
            {
                assert (!m_freeList.is_in_stack(*node));
                ++count;
                --timerNode.m_repeatCount;

                m_tickIterator = m_bucket[pos].erase(m_tickIterator);
                
                RunTask(timerNode.m_taskData);
                
                if (0 != timerNode.m_repeatCount)
                {   //currTick为GM调整后的未来时间, 以此作为下一个Timer的Tick槽
                    //这样可以避免在追赶Tick时产生新的Timer堆积, 避免CPU 100%
                    timerNode.m_expire = currTick + timerNode.m_period;
                    m_bucket[HashTick(timerNode.m_expire)].push_back(node);
                }
                else
                {
                    DecTimerCount(timerNode.m_timerType);
                    assert (!m_freeList.is_in_stack(*node));
                    ITimerTask* task = reinterpret_cast<ITimerTask*>(timerNode.m_taskData);
                    task->~ITimerTask();
                    m_freeList.push_back(*node);
                }
            }
            else
            {
                ++m_tickIterator;
            }
        }
        return count;
    }

    CTimerQueue* CTimerQueue::CreateTimerQueue( char* buffer, size_type& buffer_size, size_type count, bool resume )
    {
        assert(count > 0 && count < MAX_TIMER_NODE_NUM);

        size_type tq_size = sizeof(CTimerQueue) + (count) * sizeof(NodeType);
        if (tq_size > buffer_size)
        {
            char errorDesc[2048];
            sprintf(errorDesc, "Insufficient memory buffer size, "FMT_SIZET" needed, but only "FMT_SIZET" bytes", tq_size, buffer_size);
            throw base_exception(errorDesc);
        }

        buffer_size = tq_size;
        CTimerQueue* tq = new (buffer) CTimerQueue;
        NodeType* node = tq->m_nodeBase;

        {
            //0节点单独处理
            if (!resume)
            {
                node->m_data.m_timerSeq = 1;    //NORMAL启动时赋初值
            }

            ++node;
        }

        for (size_t i = 1; i < count; ++i, ++node)
        {
            // 把后面的对象创建起来
            new (node)NodeType;
            if (!resume)
            {
                node->m_data.m_timerSeq = 1;    //NORMAL启动时赋初值
            }
        }

		jy::framework::UpdateCurrTime();
		timeval &tv = jy::framework::s_currentTimeVal;

        if (resume)
        {
            assert(tq->m_count == count);
            framework::s_currentTick = tq->m_currentTick;
            tq->m_freeList.rebind_array_addr(tq->m_nodeBase);
            for (int i = 0; i < C_BUCKET_COUNT; ++i)
            {
                tq->m_bucket[i].resume();
            }

            NodeType* node = tq->m_nodeBase;
            for (size_type i = 0; i < count; ++i, ++node)
            {
                if (!tq->m_freeList.is_in_stack(*node))
                {
                    Resume(node->m_data);
                }
            }
        }
        else
        {
            tq->m_systemStart = tv.tv_sec;
            g_systemStart = tq->m_systemStart;
            framework::s_currentTick = tq->m_currentTick = 0;

            tq->m_count = count;
            tq->m_freeList.reset(tq->m_nodeBase, tq->m_count);
            for (int i = 0; i < C_BUCKET_COUNT; ++i)
            {
                tq->m_bucket[i].init();
            }
        }
		g_systemStart = tq->m_systemStart;
        //WRITE_INFO_LOG("CreateTimerQueue count:"FMT_SIZET", size:"FMT_SIZET", system_start:%ld, current_tick:%u, ", count, tq_size, tq->m_systemStart, tq->m_currentTick);
        //`WRITE_INFO_LOG("CreateTimerQueue FreeList base:%p, top:"FMT_SIZET"", tq->m_freeList.get_base(), tq->m_freeList.get_top());
        //统计数据初始化
        tq->InitTimerCount();
        tq->SetGlobalRunFlag(NULL);

        return tq;
    }
    void CTimerQueue::AddTimerCount(int timerType)
    {
        ++m_timerCount[timerType];
    }
    void CTimerQueue::DecTimerCount(int timerType)
    {
        if(0 >= m_timerCount[timerType])
        {
            return;
        }
        --m_timerCount[timerType];
    }
}

