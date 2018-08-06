/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：obj_desc.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月04日
*	  描述:		
*
================================================================*/


#pragma once

#include <cstddef>
#include "common/platform/common.h"

namespace jy
{
	struct tagObjID;
	typedef tagObjID IDType;
	class CObjPoolBase;

	typedef ptrdiff_t OffsetType;	//共享内存中的偏移量，可以在重新使用共享内存的时候保持不变
	OffsetType GetOffset(const void *obj);
	char* GetPtrByOffset(OffsetType offset);
    // 则所有在共享内存中创建的对象,如果需要返回IDType, 则必须从此对象派生
    // 为了防止ID重复使用带来异常，全部使用IDType作为ID来验证消息的合法性
    class tagObjectBase
    {
    private:
        unsigned int m_type : 8; //== 0 表示对象没有使用
        unsigned int m_seq : 24; 
        unsigned int m_md5;      // 空闲对象内容的MD5， 为了不改变当前的内存布局，仅使用4字节记录MD5值的前四个字节，足够识别任何的内容改变

    public:
        inline unsigned int GetObjType() const
        {
            return m_type;
        }

        inline void _SetObjType(unsigned int type)
        {
            m_type = type;
        }
        
        inline void IncSeq()    //递增序列号
        {
            if (m_seq > 0X007FFFFF) { m_seq = 0; }
            ++m_seq;
        }

        inline unsigned int GetSeq() const
        {
            return m_seq;
        }

        //防止重启的时候默认初始化共享内存为0
        tagObjectBase()
        {
        }

        virtual ~tagObjectBase()
        {
            m_type = 0;
        }

        inline IDType GetID() const;	//因为先后顺序，这里不能给予实现,虽然就一行代码

        void CalculateMd5(int dataLen);

        bool VerifyMd5(int dataLen) const;
    };

	struct tagObjID
	{
        /// 原来是OffsetType，由于在64位下OffsetType是long，因此tagObjID会超过8个字节，因此这里改为unsigned int
        /// 而现在m_id是(reinterpret_cast<const char*>(obj) - m_s_pool_start)，因此能保证obj必然大于m_s_pool_start
        /// 因此这里用unsigned int应该是没问题的
		unsigned int id;
		unsigned int type : 8;	// equal to 0表示不用检查数据是否特定的对象以及分配的序号
		unsigned int seq : 24;

        tagObjID(const tagObjID& obj_id)
            : id(obj_id.id), type(obj_id.type), seq(obj_id.seq)
        {
        }

        // 这个构造函数是为了在共享内存resume而存在
        tagObjID()
        {
        }

		explicit tagObjID(const tagObjectBase *obj) 
			: id(reinterpret_cast<const char*>(obj) - g_mem_pool_start), type(obj->GetObjType()), seq(obj->GetSeq())
		{
		}

        tagObjID& operator=(const tagObjID& obj_id)
        {
            if (this != &obj_id)
            {
                id = obj_id.id;
                type = obj_id.type;
                seq = obj_id.seq;
            }
            return *this;
        }

        inline bool operator==(const tagObjID& obj_id) const
        {
            return (id == obj_id.id 
                && ((0 == type || 0 == obj_id.type)
                || (type == obj_id.type && seq == obj_id.seq)));
        }		

        inline bool operator!=(const tagObjID& obj_id) const
        {
            return !(*this == obj_id);
        }

		static unsigned int ID2Type(uint64 id)
		{
			return (((unsigned int)id) >> 24) & 0xff;
		}

		inline operator uint64() const
		{
			return ((uint64)id << 32) + (type << 24) + seq;
		}

		inline unsigned int GetType() const
		{
			return type;
		}

		char* PoolStart()
		{
			return g_mem_pool_start;
		}

		static tagObjID NilID()
		{
			tagObjID obj_id;
			obj_id.id = 0;
			obj_id.type = 0;
			obj_id.seq = 0;
			return obj_id;					
		}

		template<typename T>
		T* GetObject() const
		{
			if(0 == id)
			{
				return NULL;
			}
			T *p = reinterpret_cast<T*>(tagObjID::g_mem_pool_start + id);
			if((char*)p > g_mem_pool_end || 0 == p->GetObjType() ||
				 type != p->GetObjType() || seq != p->GetSeq())
			{
				p = NULL;
			}
			return dynamic_cast<T*>((struct tagObjectBase*)p);
		}
	private:
		friend class CObjPoolMgr;

        static char* g_mem_pool_start; 
        static char* g_mem_pool_end;
	};

	typedef struct tagObjIDRannge
	{
		OffsetType mem_pool_start;
		OffsetType mem_pool_end;
		unsigned int unit_size;
		CObjPoolBase *pool;

		tagObjIDRannge()
		{
			pool = NULL;
		}
		
		void Reset(CObjPoolBase *pool);
		
		inline bool IsValid(OffsetType offset)
		{
			return 1 != unit_size && offset >= mem_pool_start && offset <= mem_pool_end && 0 == ((offset - mem_pool_start) % unit_size);
		}
	}tagObjIDRannge;

	// 系统启动的方式。这个变量会决定所有在共享内存中对象的启动模式
	bool IsResume();
	long ResumeFix();

    // 重新设置系统为初始化运行方式。系统完成共享内存初始化后启动后必须使用此方式来创建对象
    void ResetInitlize();

	void SetResume(bool resume);
	inline IDType tagObjectBase::GetID() const
	{
    	return IDType(this);
	}

}
