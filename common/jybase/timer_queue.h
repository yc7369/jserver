/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：timer_queue.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月05日
*	 描述:		
*
================================================================*/


#pragma once


#include "obj_desc.h"
#include "array_list.h"
#include "sys_time.h"
#include "../base/run_flags.h"

namespace jy
{
    struct RunCostNode
    {
        long long m_totalTime;
        long long m_totalCount;
    };

	time_t GetSystemStartTime();
    void SetTraceFlag();
    void ClrTraceFlag();
    
    
    // 即便不做任何操作，也要生命此函数；否则vc下面debug表现不正常，因为vc在
    // debug默认是初始化为0
    struct tagTimerNode
    {
        tagTimerNode()
        {
        }

        static const int MAX_OBJ_LENGTH = TIMER_DATA_LEN; 
        char m_taskData[MAX_OBJ_LENGTH];    // 存储从ITimerTask继承的数据
        uint32 m_expire;                    // 定时器到期时间
        uint32 m_period;                    // 定时器时间重复间隔
        uint32 m_repeatCount;       // 定时器重复次数. == 0 直接删除而不是触发对象时
        unsigned short m_timerType;         // 定时器类型。主要用于resume
        unsigned short m_timerSeq;          // 定时器节点序号
    };

    class ITimerTask;

    // 暂时使用数组来存储定时器信息，以便现将精力放在其他地方
    // 数组存储信息虽然空间占用较多，而且性能方便一般，但是使用起来最为简单
    class CTimerQueue
    {
    public:
        typedef std::size_t size_type;
        static const uint32 C_PERMANENT_REPEAT = 0xFFFFFFFF;
        static const int    C_BUCKET_COUNT = C_TICK_COUNT_PER_SECOND * 60 * 30+1; // 30分钟，每个tick一个bucket
        static const uint64 C_INVALID_TIMER_ID = 0;
        static const uint32 MAX_TIMER_NODE_NUM = 1000000;       //最多允许的TIMER NODE 数

        //************************************
        // Method:    Schedule 设置定时器
        // FullName:  jy::CTimerQueue::Schedule
        // Access:    public 
        // Returns:   uint32 C_INVALID_TIMER_ID失败，其他对应为timerID，方便Cancel
        // Qualifier:
        // Parameter: const ITimerTask & task 定时器任务
        // Parameter: uint32 delay  定时器第一次触发的时间间隔(相对时间)
        // Parameter: uint32 period 定时器重复执行的时间间隔
        // Parameter: uint32 repeat_count 重复的次数
        //************************************
        uint64 Schedule(const ITimerTask& task, uint32 delay, uint32 period, uint32 repeat_count);
        
        uint64 Schedule(const ITimerTask& task, uint32 delay, uint32 repeat_count)
        {
            return Schedule(task, delay, delay, repeat_count);
        }

        uint64 ScheduleOnce(const ITimerTask& task, uint32 delay)
        {
            return Schedule(task, delay, 1);
        }

        //************************************
        // Method:    Cancel 取消定时器
        // FullName:  jy::CTimerQueue::Cancel
        // Access:    public 
        // Returns:   void
        // Qualifier:
        // Parameter: uint32 & id Schedule或者ScheduleOnce返回的ID，函数处理完成后id=C_INVALID_TIMER_ID
        //************************************
        void Cancel(uint64& timerid);
        
        int Tick();
        
        int SkipTick();     //配合GM调时间, 向前跳过若干Tick
        
        void LogCostStat();     //Timer性能监视辅组日志, glory, 稳定后不会记录

        static CTimerQueue* CreateTimerQueue(char* buffer, size_type& buffer_size, size_type count, bool resume);

        static inline uint32 TimerId2Pos(uint64 timerid)
        {
            uint32 pos = 0;
            unsigned short seq = 0;
                   unsigned short type = 0;
            DecodeTimerId(timerid, type,  pos , seq);
            return pos;
        }

        int* GetTimerCount()//int * timerCount)
        {
            return m_timerCount;
            //timerCount = m_timerCount;
        }
        void AddTimerCount(int timerType);
        void DecTimerCount(int timerType);
        void InitTimerCount()
        {
            memset(m_timerCount,0,sizeof(m_timerCount));
        }

        void SetGlobalRunFlag(CRunFlag* runFlag)
        {
        	m_runFlag = runFlag;
        }
        
    protected:
        typedef array_list_node<tagTimerNode> NodeType;
        typedef array_list<tagTimerNode> ArrayListType;
        typedef stack_array_list<tagTimerNode> FreeNodeStackType;

        // 因为本类需要在类后面使用一块空间，因此不直接提供public的构造函数
        // 防止将本类作为其他类的一个成员而引发异常
        CTimerQueue()
        {
        	m_runFlag = NULL;
        }

        inline int HashTick(uint32 totalCount)
        {
            return totalCount % C_BUCKET_COUNT;
        }

        /*
        为避免TIMER被误释放, 在TIMER节点中增加SEQ字段(0,60000),
        并调整TIMER ID 的生成规则, 由TIMER NODE POS和SEQ, TIMER TYPE共同组成, 
        */
        static inline uint64 EncodeTimerId( unsigned short timer_type, uint32 pos, unsigned short seq)
        {
            assert(seq > 0 && seq < 60000);
            assert(pos > 0 && pos <= MAX_TIMER_NODE_NUM);
            
            uint64 timerid = timer_type ;
            timerid = timerid << 16;
            
            timerid += seq;
            timerid = timerid << 32;
            
            timerid += pos;
            return timerid;
        }

        static inline int DecodeTimerId(uint64 timerid,  unsigned short& timer_type, uint32& pos , unsigned short & seq)
        {
            pos = timerid & 0XFFFFFFFF;
            seq = (timerid >> 32) & 0XFFFF;
            timer_type = timerid >> 48;

                   assert(timerid != pos);

            if ((pos <= 0 || pos > MAX_TIMER_NODE_NUM) ||
                (seq <= 0 || seq >= 60000))
            {
                return -1;
            }
            return 0;
        }

        static inline void StepUpTimerSeq(unsigned short& seq)
        {
            assert(seq > 0 && seq < 60000);
            ++seq;
            if (seq >= 60000 || seq <= 0)
            {
                seq = 1;
            }
        }

        int CheckTickArray(uint32 tick, int pos);
        
        //配合GM调时间, 向前跳过若干Tick
        int SkipTickArray(uint32 tick, int pos);

        int DoBadTimeList(int pos, ArrayListType::iterator curItr, bool doFix=true);

        bool IsValidNodePointer(NodeType* node);

        
        CRunFlag* m_runFlag;

	
        //TIMER个数统计
        static const uint32 MAX_TIMER_NUM = 200;
        int m_timerCount[MAX_TIMER_NUM];  //正在使用的各类别timer个数,暂时先定70，如果timer数量超过了，需要改变

        size_type m_count; // 实际最多可以分配的对象个数
        ArrayListType m_bucket[C_BUCKET_COUNT]; // 使用队列

        // 因为在tick的时候会调用其他函数,而这些函数可能会修改即将tick的iterator
        // 因此在类里面保留此信息，这样cancel函数导致iterator失效的话，可以得到合适的数值
        ArrayListType::iterator m_tickIterator; // 当前tick使用的iterator
        FreeNodeStackType m_freeList; // 空闲队列
        uint32  m_currentTick; // 当前tick的时间
        long    m_systemStart; // 系统开始的时间（单位为秒）
        NodeType m_nodeBase[1];
        
        
    };

};/* jy_LIB_TIMER_QUEUE_HEADER_ */


