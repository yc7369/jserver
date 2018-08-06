/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：obj_pool.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月05日
*	描述:对象池管理，包括对象分配、释放以及对已经分配的所有对象做特定的操作
*
================================================================*/
#pragma once

#include "buffer_array.h"
#include "obj_desc.h"
#include "base_exception.h"

#ifndef ALIGN_SIZE
	#define ALIGN_SIZE(Size, align) ((align - 1 + Size) & (~(align - 1)))
	#define ALIGN_POINT(pointer, align) ALIGN_SIZE((reinterpret_cast<std::size_t>(pointer)), align)
#endif

namespace jy
{
	template<typename T>
	struct tagDefConstructor
	{
		void  Construct(void *p)
		{
			::new(p)T();
		}

		void  Construct(void *p, const T& val)
		{
			::new(p)T(val);
		}

		void Destroy(void *p)
		{
			T *tmp = reinterpret_cast<T*>(p);
			tmp->~T();
		}
	};

	class CObjPoolBase
	{
	public:
		typedef std::size_t size_type;
		static const int BUSY_NODE_INDEX = 0;
		static const int TAIL_OF_LIST = -1;
		static const size_type MAX_ALLOCATE_COUNT = 0x0fffffff;
		static size_type PoolSize(size_type unit_size, size_type count);

        //************************************
        // Method:    Size 获取已经分配对象的个数
        // FullName:  xxz::CObjPoolBase::Size
        // Access:    public 
        // Returns:   xxz::CObjPoolBase::size_type 已经分配对象的个数
        // Qualifier: const
        //************************************
        inline size_type Size() const
        {
            assert(allocate_count_ >= deallocate_count_);
            return allocate_count_ - deallocate_count_;
        }

        //************************************
        // Method:    MaxSize 获取最大可以分配对象的个数
        // FullName:  xxz::CObjPoolBase::MaxSize
        // Access:    public 
        // Returns:   xxz::CObjPoolBase::size_type 最大可以分配对象的个数
        // Qualifier: const
        //************************************
        inline size_type MaxSize() const
        {
            return array_.MaxSize();
        }  

		inline size_type UnitSize() const
		{
			return unit_size_;
		}

		inline size_type AllocateCount() const
		{
			return allocate_count_;
		}

		inline size_type DeallocateCount() const
		{
			return deallocate_count_;
		}

		unsigned char PoolType() const
		{
			return obj_type_;
		}

		const char* PoolStart() const
		{
			return array_.Start();
		}

		inline size_type MaxUsedNode() const
		{
			return max_used_node_;
		}

		inline bool IsFree(unsigned int index) const
		{
			if(index >= count_) return false;
			return bit_map_[index] != BUSY_NODE_INDEX;
		}

		inline const void* GetDataPtr(unsigned int index) const
		{
			if(index >= count_) return NULL;
			return (const void*)array_[index];
		}
    protected:
        // 因为本类需要在类后面使用一块空间，因此不直接提供public的构造函数
        // 防止将本类作为其他类的一个成员而引发异常
        void CreatePool(unsigned int obj_type, size_type unit_size, size_type count, bool resume);

		inline void ConstructFreeList()
		{
			for(size_type i = count_; i > 0; )
			{
				SetFreeNode(--i);
#ifdef ENABLE_AUDIT
				((tagObjectBase *)array_[i])->CalculateMd5(UnitSize());
#endif
			}
		}

		void FixFreeList()
		{
			free_node_ = TAIL_OF_LIST;
			for(size_type i = count_; i > 0; )
			{
				--i;
				if(BUSY_NODE_INDEX != bit_map_[i])
				{
					SetFreeNode(--i);
				}
			}
		}

		inline void SetFreeNode(size_type index)
		{
			bit_map_[index] = free_node_;
			free_node_ = static_cast<int>(index + 1);
		}

		inline void * GetFreeNode()
		{
			if(free_node_ == TAIL_OF_LIST)
			{
				return NULL;
			}

			if(free_node_ > max_used_node_)
			{
				max_used_node_ = free_node_;
			}

			size_type curr_node = free_node_ - 1;
			void *r = array_[curr_node];

			if(BUSY_NODE_INDEX == bit_map_[curr_node])
			{
				FixFreeList();
				return NULL;
			}

			free_node_ = bit_map_[curr_node];
			bit_map_[curr_node] = BUSY_NODE_INDEX;
			return r;
		}
		unsigned char obj_type_; // check when resume and check when use id
		BufferArray array_;	
		size_type allocate_count_;
		size_type deallocate_count_;

		size_type unit_size_;
		size_type count_;

		int free_node_;
		int max_used_node_;
		int bit_map_[1]; // equal to 0 means used! others for next free node index(base on 1)
	};

    // 由于释放对象时并不会按照已使用队列的顺序依次来操作
    // 这一点和空闲队列是不一样的。对于空闲队列只有分配新空队列和插入新node的两个操作
    // 可以按照顺序分配
    // 但是对于已分配队列，必须提供随机访问的接口。而list的随机访问的操作为O(n)，操作过重
    // 如果使用类似于盛唐的list base on array的方式，又因为是双向list,不算是特别理想
    // 因此对于已分配队列采用数组的方式而不是队列的方式。
    // 在TObjPool析构函数里面没有调用已经分配对象的析构函数，这是因为以下原因：
    // 1. 分配对象不应该从其他堆上分配资源。即对象的析构函数应该是做其他的清理工作，比如玩家断开连接
    // 2. 对于其他资源由系统来释放。防止这里的调用引发没有考虑到的异常。即资源的回收需要由系统明确要求来做
    // 由于CObjPoolBase需要说使用类后面的内存数据，因此TObjPool只是增加接口而不是增加对象数
	template<class T>	
	class TObjPool : public CObjPoolBase
	{
	public:
        //************************************
        // Method:    ForEach 对[first, last)范围内的所有对象，按照状态调用函数
        //        提供ForEach函数而不是提供iterator是因为如果提供iterator,则需要提供已经分配和没有分配的
        //        两类iterator，不容易理解。Function有一个输入参数是T&
        // FullName:  xxz::TObjPool<Function>::ForEach
        // Access:    public 
        // Returns:   Function
        // Qualifier:
        // Parameter: size_type first 起始的下表索引(>=0)
        // Parameter: size_type last  最终的下表索引 (<= m_max_count + 1)
        // Parameter: Function func   调用的函数
		template<class Function>
		inline Function ForeachObj(size_type first, size_type last, Function func, size_type* max_for_num = NULL)
		{
	        // perform function for each element
            //assert(first >= 0);
			assert(first <= last && last <= array_.MaxSize());
			
			size_type for_num = 0;
			if(last > MaxUsedNode())
			{
				last = MaxUsedNode();
			}
		
			for(; first != last; ++first)
			{
				if(BUSY_NODE_INDEX == bit_map_[first])
				{
					int ret = func(reinterpret_cast<T*>(array_[first]));

					++for_num;
					if(max_for_num)
					{
						if(for_num >= *max_for_num)
						{
							break;
						}
					}

					if(ret <= 0)
					{
						break;
					}
				}
			}
			if(max_for_num)
			{
				*max_for_num = for_num;
			}
			return func;
		}

		T* GetBySeq(size_type seq)
		{
			if(BUSY_NODE_INDEX == bit_map_[seq])
			{
				return reinterpret_cast<T*>(array_[seq]);
			}
			return NULL;
		}

		size_t GetNodeSeq(const T *ptr)	const
		{
			return array_.Distance(reinterpret_cast<const char*>(ptr));
		}

		inline void Deallocate(T* obj)
		{
			if(obj)
			{
				size_type index = array_.Distance(reinterpret_cast<char*>(obj));
				if(BUSY_NODE_INDEX != bit_map_[index])
				{
					//ERROR();
					return ;
				}

				++deallocate_count_;
				SetFreeNode(index);
				tagDefConstructor<T> constructor;
				constructor.destroy(obj);

#ifdef ENABLE_AUDIT
				tagObjectBase *pobj = (tagObjectBase*)obj;
				pobj->CalculateMd5(UnitSize());
#endif
			}
		}

		inline T* Allocate()
		{
            //resume重启过程中, 不允许创建新的共享内存对象, 
            //否则新对象构造函数按resume模式执行, 很多字段没有赋初值
			//assert(jy::IsResume() == false);
			T* p = reinterpret_cast<T*>(GetFreeNode());
			if(p)
			{
				++allocate_count_;
				if(allocate_count_ >= MAX_ALLOCATE_COUNT)
				{
					assert(allocate_count_ >= deallocate_count_);
					allocate_count_ -= deallocate_count_;
					deallocate_count_ = 0;
				}
				
				p->IncSeq();
				p->SetObjType(obj_type_);

				tagDefConstructor<T> constructor;
				constructor.Construct(p);
			}
			return p;
		}

		static TObjPool* CreateObjpool(char* buffer, size_type& buffer_size, unsigned int obj_type, size_type unit_size, size_type count, bool resume)
		{
			size_type need_capacity = PoolSize(unit_size, count);
			if(buffer_size < need_capacity)
			{
				char err_desc[2048];
				sprintf(err_desc, "Insufficient memory buffer size, " FMT_SIZET " needed, but only " FMT_SIZET " bytes", need_capacity, buffer_size);
                throw base_exception(err_desc);
			}

	        TObjPool* pool = new(buffer) TObjPool(obj_type, unit_size, count, resume);
            buffer_size = need_capacity;

			return pool;
		}
	private:
        // 因为本类需要在类后面使用一块空间，因此不直接提供public的构造函数
        // 防止将本类作为其他类的一个成员而引发异常
        TObjPool(unsigned int obj_type, size_type unit_size, size_type count, bool resume)
        {
            CreatePool(obj_type, unit_size, count, resume);
            if (resume)
            {
                ResumeNode();
            }
            else
            {
                ConstructFreeList();
            }
        }


        inline void ResumeNode()
        {
            free_node_ = TAIL_OF_LIST;    //将FREE LIST先清空， 再重新一个个FREE NODE往里面塞
            tagDefConstructor<T> constructor;
            for (size_type i = count_; i > 0; )
            {
                --i;
                if (BUSY_NODE_INDEX != bit_map_[i])
                {
                    SetFreeNode(i);
                }
                else
                {
                    constructor.Construct(array_[i]);
                }
            }
        }		
	};


    class CTimerQueue;
    class CObjPoolMgr
    {
    public:
        typedef std::size_t size_type;
        CObjPoolMgr(char* buffer, size_type capacity, bool resume)
            : m_nextPoolBuffer(buffer), m_capacity(capacity), m_tq(0), m_resume(resume)
        {
            m_ResumeFix =0;
            if(m_resume)
            {
                char *OrigPtr = *((char **)m_nextPoolBuffer);
                m_ResumeFix = m_nextPoolBuffer - OrigPtr;
//                LOG_INFO("Resume fixed offset [%ld],OrigPtr[%p] NewPtr[%p]",m_ResumeFix,OrigPtr,m_nextPoolBuffer);
            }

            *((char **)m_nextPoolBuffer) = m_nextPoolBuffer;
            m_nextPoolBuffer += ALIGN_SIZE(sizeof(char *), 8);

            InitPoolMgr();
        }

        //************************************
        // Method:    CreatePool 构造object pool
        // FullName:  xxz::CObjPoolMgr<T>::CreatePool
        // Access:    public 
        // Returns:   int -1表示空间不够。0表示成功。如果已经存在相同类型的对象，则抛出异常
        // Qualifier:
        // Parameter: unsigned int obj_type 对象类型，必须全局唯一。即不能创建两个相同类型的对象
        // Parameter: size_type unit_size 对象的大小
        // Parameter: size_type count 对象的个数
        //************************************
        template <class T>
        int CreatePool(unsigned int obj_type, size_type unit_size, size_type count)
        {
            size_type capacity(m_capacity);
            TObjPool<T>* pool = TObjPool<T>::CreateObjpool(m_nextPoolBuffer, capacity, obj_type, unit_size, count, m_resume);
            if (0 == pool)
            {
                return -1;
            }
            InsertPool(obj_type, pool);
            m_capacity    -= capacity;
            m_nextPoolBuffer += capacity;
            return 0;
        }

        //************************************
        // Method:    CreateObj 创建对象，用于单一的从共享内存中创建的对象，比如全局唯一的CAsynQueue
        // FullName:  xxz::CObjPoolMgr<T>::CreateObj
        // Access:    public 
        // Returns:   int
        // Qualifier:
        // Parameter: size_type unit_size 需要的大小
        //************************************
        template <class T>
        int CreateObj(size_type unit_size)
        {
            unit_size = ALIGN_SIZE(unit_size, 4);
            if (m_capacity < unit_size)
            {
                char errorDesc[2048];
                sprintf(errorDesc, "Insufficient memory buffer size, " FMT_SIZET " needed, but only " FMT_SIZET " bytes", unit_size, m_capacity);
                throw base_exception(errorDesc);
            }
            new (m_nextPoolBuffer) T;
            m_capacity    -= unit_size;
            m_nextPoolBuffer += unit_size;

            //LOG_INFO("CreateObj UnitSize<" FMT_SIZET "> , capacity:" FMT_SIZET "",unit_size, m_capacity);

            return 0;
        }

        //************************************
        // Method:    CreateBuffer 仅用于分配空间，不调用构造函数，需要外部来处理的
        // FullName:  xxz::CObjPoolMgr<T>::CreateBuffer
        // Access:    public 
        // Returns:   void* 缓冲区指针
        // Qualifier:
        // Parameter: size_type unit_size 需要的大小
        //************************************
        void* CreateBuffer(size_type unit_size)
        {
            unit_size = ALIGN_SIZE(unit_size, 4);
            if (m_capacity < unit_size)
            {
                char errorDesc[2048];
                sprintf(errorDesc, "Insufficient memory buffer size, " FMT_SIZET " needed, but only " FMT_SIZET " bytes", unit_size, m_capacity);
                throw base_exception(errorDesc);
            }
            char* ret = m_nextPoolBuffer;
            m_capacity    -= unit_size;
            m_nextPoolBuffer += unit_size;

            //LOG_INFO("CreateBuffer UnitSize<" FMT_SIZET "> , capacity:" FMT_SIZET "\n",unit_size, m_capacity);
            
            return ret;
        }

        //************************************
        // Method:    GetPool 获取对象管理池。如果该类型不存在，则抛出异常
        // FullName:  xxz::CObjPoolMgr::GetPool
        // Returns:   CObjPool& 其他为实际对象管理池
        // Parameter: unsigned int obj_type 对象类型
        //************************************
        template <class T>
        TObjPool<T>& GetPool(unsigned int obj_type)
        {
            return *reinterpret_cast<TObjPool<T> *>(FindPool(obj_type));
        }

        CTimerQueue* CreateTimerQueue(size_type count);
        CTimerQueue* CreateTimerQueue4MProtect(size_type count);

        inline size_type size() const
        {
            return m_capacity;
        }

        CTimerQueue* TimerQueue()
        {
            return m_tq;
        }

        void PrintStatus() const;
        int  DumpStatus(char * buffer, int buffLen) const;

        void AuditFreeObjs();

        void* Allocate(size_t size)
        {
            char* buffer = m_nextPoolBuffer;
            assert(size <= m_capacity);
            if( size > m_capacity )
            {
                printf("Allocate Failed " FMT_SIZET " Bytes!\n", size - m_capacity);
                return NULL;
            }
            m_capacity    -= size;
            m_nextPoolBuffer += size;
            return buffer;
        }
        long GetResumeFix()const{return m_ResumeFix;};
    private:
        void InsertPool(unsigned int obj_type, CObjPoolBase* pool);
        void* FindPool(unsigned int obj_type);
        void* SearchPool(unsigned int obj_type);
        void InitPoolMgr();
        char* m_nextPoolBuffer;
        size_type m_capacity;
        CTimerQueue* m_tq;
        bool m_resume;
        long m_ResumeFix;

        unsigned int m_lastAuditType;
        unsigned int m_lastAuditIndex;
    };
}

