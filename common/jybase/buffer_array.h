/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：buffer_array.h
*   创 建 者：Jeson Yang
*   创建日期：2018年08月05日
*	 描述:		
*
================================================================*/


#pragma once

#include <string>
#include <assert.h>

namespace jy
{
	class BufferArray
	{
	public:
		typedef std::size_t size_type;
		
		static size_type BufferSize(size_type unit_size, size_t count)
		{
			return unit_size * count;
		}

		BufferArray()
			:start_(0), unit_size_(0), count_(0)
		{
		}

		void InitArray(char* start, size_type unit_size, size_type count)
		{
			start_ = start;
			unit_size_ = unit_size;
			count_ = count;
		}

		size_type MaxSize() const
		{
			return count_;
		}
		const char* Start() const
		{
			return start_;
		}

		size_type Distance(const void* x) const
		{
			const char* p = reinterpret_cast<const char*>(x);
			assert(p >= start_);
			assert(0 == (p - start_) % unit_size_);
			assert((p - start_) / unit_size_ < count);
			return (p - start_) / unit_size_;
		}

		void* operator[](size_type index)
		{
			return start_ + index * unit_size_;
		}

		const void* operator[](size_type index) const
		{
			return start_ + index * unit_size_;
		}

		void* At(size_type index)
		{
			assert(index < count_);
			return start_ + index * unit_size_;
		}

		const void* At(size_type index) const
		{
			assert(index < count_);
			return start_ + index * unit_size_;
		}

	private:
		char* start_;
		size_type unit_size_;
		size_type count_;
	};
}
