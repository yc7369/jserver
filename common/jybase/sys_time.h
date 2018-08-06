#ifndef JY_LIB_SYS_TIMER_HEADER_
#define JY_LIB_SYS_TIMER_HEADER_

namespace jy
{
	const int C_TICK_COUNT_PER_SECOND = 20; // 定时器精度。即每秒做多少次tick。
    
    //inline uint32 TickToMs(uint32 tick)     //Tick转化为时间毫秒，时间段不能超过4天，否则会溢出
    //{
    //    return tick * (1000 / C_TICK_COUNT_PER_SECOND);
    //}
    
    //inline uint32 MsToTick(uint32 ms)       //毫秒转化为Tick
    //{
    //    return ms / (1000 / C_TICK_COUNT_PER_SECOND);
    //}
    
    //inline uint32 TickToSec(uint32 tick)    //Tick转化为秒
    //{
    //    return tick / C_TICK_COUNT_PER_SECOND;  
    //}
    
    //inline uint32 SecToTick(uint32 sec)     //秒转化为Tick
    //{
    //   return sec * C_TICK_COUNT_PER_SECOND; 
    //}
    
    //Tick转化为时间毫秒，时间段不能超过4天，否则会溢出
    inline int32 TickToMs(int32 tick) { return tick * (1000 / C_TICK_COUNT_PER_SECOND); }

    //毫秒转化为Tick
    inline int32 MsToTick(int32 ms) { return ms / (1000 / C_TICK_COUNT_PER_SECOND); }
    
    //Tick转化为秒
    inline int32 TickToSec(int32 tick) { return tick / C_TICK_COUNT_PER_SECOND; }
    
    //秒转化为Tick
    inline int32 SecToTick(int32 sec) { return sec * C_TICK_COUNT_PER_SECOND; }
    
    //返回time2与time1的差值，单位毫秒；即time2 - time1
    inline int CompareTimeVal(const struct timeval & time1, 
        const struct timeval & time2)
    {
        return (time2.tv_sec - time1.tv_sec)*1000 
            + (time2.tv_usec - time1.tv_usec)/1000;
    }
    
    inline int GetDiffTimeUs(const struct timeval & time1,
        const struct timeval & time2)
    {
        return (time2.tv_sec == time1.tv_sec) ? (time2.tv_usec - time1.tv_usec) : 
            ((time2.tv_sec - time1.tv_sec)*1000000 + (time2.tv_usec - time1.tv_usec));
    }

	namespace framework
	{
        //获取当前的Tick值
		uint32 GetCurrentTick();

        //获取当前时间值, 相当于time(NULL)
        time_t GetCurrentTime();    
        
        //获取系统TimeVal, 直接从s_currentTimeVal取得, 减少系统调用, 返回值为当前的秒数
        time_t GetCurrentTimeVal(struct timeval * tv);
        
        //获取系统TimeVal, 返回值为当前的微妙数, (glory:挺奇怪的一个函数 )
		time_t GetCurrentTimeUsec(struct timeval *tv);
        
        // 获取系统时间(单位ms), (glory:建议不用, 可以用Tick获取高于秒的精度)
		int GetCurrentMs();

		//获取系统TimeVal, 赋值给s_currentTimeVal
		time_t TickCurrentTimeVal(struct timeval * tv);
        
        //更新server cache的时间
        void UpdateCurrTime();
        
        //设置时间增量, 用于实现GM调整时间
        void SetTimeIncrement(int addSec);   
        
        //获取时间增量
        int GetTimeIncrement();
        
	}
}

#define ONE_SECOND_IN_MSECS  (1000L)
#define ONE_SECOND_IN_USECS  (1000000L)
#define ONE_MSECOND_IN_USECS (1000L)

// 两个时间之差别,单位us
inline long operator- (const timeval& left, const timeval& right)
{
	long secs = left.tv_sec - right.tv_sec;
	if (secs > INT_MAX / ONE_SECOND_IN_USECS)
	{
		return INT_MAX;
	}
	return secs * ONE_SECOND_IN_USECS + (left.tv_usec - right.tv_usec);
}

inline bool operator== (const timeval& left, const timeval& right)
{
	return left.tv_sec == right.tv_sec && left.tv_usec == right.tv_usec;
}

inline bool operator< (const timeval& left, const timeval& right)
{
	return left.tv_sec < right.tv_sec
		|| (left.tv_sec == right.tv_sec && left.tv_usec < right.tv_usec);
}

inline bool operator<= (const timeval& left, const timeval& right)
{
	return left < right || left == right;
}

inline bool operator> (const timeval& left, const timeval& right)
{
	return right < left;
}

inline bool operator>= (const timeval& left, const timeval& right)
{
	return right < left || left == right;
}

#endif /* XXZ_LIB_SYS_TIMER_HEADER_ */


