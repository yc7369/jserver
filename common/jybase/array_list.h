/*================================================================
  @file: array_list.h,v 1.0 2008-04-08 $
  @author johnzhang
  @brief  一个基于数组的双向链表
           在此链表的基础上设计了一个固定大小的内存分配器
           主要是用于对象固定、对象最大个数固定的情况

  @since 2008-04-08
  Copyright (C) 2008 Tencent Co. Ltd.	
*
================================================================*/


#pragma once


namespace jy
{
#define AssertOrphanNode(node)  assert(array_list_node_base::ORPHAN_NODE_DIFF == (node).m_next && array_list_node_base::ORPHAN_NODE_DIFF == (node).m_prev)
#define AssertNonOrphanNode(node) assert(array_list_node_base::ORPHAN_NODE_DIFF != (node).m_next && array_list_node_base::ORPHAN_NODE_DIFF != (node).m_prev)

	// 为了方便快捷的删除，还是需要使用双向链表
	struct array_list_node_base
	{
		typedef std::ptrdiff_t difference_type;
		static const difference_type ORPHAN_NODE_DIFF = 0X7FFFFFFF; // 不在任何队列中
		inline bool IsOrphanNode() const
		{
			return ORPHAN_NODE_DIFF == m_next;
		}

		inline void SetOrphanNode()
		{
			m_next = m_prev = ORPHAN_NODE_DIFF;
		}

		difference_type m_next; // 下一个节点指针和当前位置的偏移，所以next_不能为0
		difference_type m_prev; // 同next_一样，也不能是0。注意对于prev需要做“-”操作
	};

	template <class _Tp>
	struct array_list_node : public array_list_node_base
	{
		_Tp m_data;
	};
	struct array_list_iterator_base
	{
		typedef std::size_t                    size_type;
		typedef std::bidirectional_iterator_tag	   iterator_category;
        typedef array_list_node_base::difference_type difference_type;
		array_list_node_base* m_node;
		
		array_list_iterator_base(array_list_node_base* __x)
			: m_node(__x) {}

		inline void incr()
		{ 
			m_node = offset_node(m_node, m_node->m_next); 
		}

		inline void decr()
		{ 
			m_node = offset_node(m_node, m_node->m_prev); 
		}

		inline array_list_node_base* offset_node(array_list_node_base* node, difference_type diff) const
		{
			return reinterpret_cast<array_list_node_base*>(reinterpret_cast<char*>(node) + diff);
		}

		inline bool operator==(const array_list_iterator_base& __x) const
		{ 
			return m_node == __x.m_node; 
		}

		inline bool operator!=(const array_list_iterator_base& __x) const
		{ 
			return m_node != __x.m_node; 
		}
	};

	template <class _Tp, class _Ref, class _Ptr>
	struct array_list_iterator : public array_list_iterator_base
	{
		typedef array_list_iterator<_Tp, _Tp&, _Tp*>             iterator;
		typedef array_list_iterator<_Tp, const _Tp&, const _Tp*> const_iterator;
		typedef array_list_iterator<_Tp, _Ref, _Ptr>             _Self;

		typedef _Tp              value_type;
		typedef _Ptr             pointer;
		typedef _Ref             reference;
		typedef array_list_node<value_type> NodeType;

		explicit array_list_iterator(array_list_node_base* __x)
			: array_list_iterator_base(__x) 
		{
		}

		array_list_iterator()
			: array_list_iterator_base(0) 
		{
		}

		array_list_iterator(const iterator& __x)
			: array_list_iterator_base(__x.m_node) 
		{
		}

		array_list_iterator(const const_iterator& __x)
			: array_list_iterator_base(__x.m_node) 
		{
		}

		reference operator*() const
		{ 
			return reinterpret_cast<NodeType*>(m_node)->m_data; 
		}

		pointer operator->() const
		{ 
			return &(operator*()); 
		}
	
		_Self& operator++()
		{
			incr();
			return *this;
		}

		_Self operator++(int)
		{
			_Self __tmp = *this;
			incr();
			return __tmp;
		}

		_Self& operator--()
		{
			decr();
			return *this;
		}

		_Self operator--(int)
		{
			_Self __tmp = *this;
			decr();
			return __tmp;
		}
	};

	template <class _Tp>
	class array_list
	{
	public:
		typedef _Tp						value_type;
		typedef value_type&             reference;
		typedef const value_type&       const_reference;
		typedef value_type*             pointer;
		typedef const value_type*       const_pointer;
		typedef array_list_iterator<value_type, reference, pointer>             iterator;
		typedef array_list_iterator<value_type, const_reference, const_pointer> const_iterator;

		typedef array_list_node_base	node_base;
		typedef array_list_node<_Tp>	node_type;

		typedef std::ptrdiff_t			different_type;

		array_list()
		{
		}
		
		// 重新使用缓冲区方式创建之后调用
		void resume()
		{
		}

		// 创建原始对象调用 
		void init()
		{
			link(&m_base, &m_base);
		}

		inline iterator begin()
		{
			return iterator(next_node(&m_base, m_base.m_next));
		}

		inline const_iterator begin() const
		{
			return const_iterator(next_node(&m_base, m_base.m_next));
		}

		inline const_iterator end() const
		{
			return const_iterator(&m_base);
		}

		inline iterator end()
		{
			return iterator(&m_base);
		}

		/*
		简化的做法：
		在双向链表中，反向迭代器与迭代器走同一套，
		在此只需要提供rbegin，rend与end相同
		*/
		inline iterator rbegin()
		{
			return iterator(next_node(&m_base, m_base.m_prev));
		}

		inline const_iterator rbegin() const
		{
			return const_iterator(next_node(&m_base, m_base.m_prev));
		}


		inline bool empty() const
		{
			return 0 == m_base.m_next;
		}

		// element access
		/**
		*  Returns a read/write reference to the data at the first
		*  element of the %list.
		*/
		reference front()
		{ 
			return *begin(); 
		}

		/**
		*  Returns a read-only (constant) reference to the data at the first
		*  element of the %list.
		*/
		const_reference front() const
		{ 
			return *begin(); 
		}

		/**
		*  Returns a read/write reference to the data at the last element
		*  of the %list.
		*/
		reference back()
		{ 
			return *iterator(next_node(&m_base, m_base.m_prev));
		}

		/**
		*  Returns a read-only (constant) reference to the data at the last
		*  element of the %list.
		*/
		const_reference back() const
		{ 
			return *const_iterator(next_node(&m_base, m_base.m_prev));
		}

		// node 必须是数组中的一位
		void push_back(node_type* node)
		{
			node_base* tail = next_node(&m_base, m_base.m_prev);
			link(tail, node);
			link(node, &m_base);
		}

		iterator erase(iterator itr)
		{
			array_list_iterator_base& base = itr;
			node_base* curr_node = base.m_node;
			AssertNonOrphanNode(*curr_node);

			node_base* prev = next_node(curr_node, curr_node->m_prev);
			node_base* next = next_node(curr_node, curr_node->m_next);
			link(prev, next);
			curr_node->SetOrphanNode();
			return iterator(next);
		}

		/**
		*  @brief  Removes last element.
		*
		*  This is a typical stack operation.  It shrinks the %list by
		*  one.  Due to the nature of a %list this operation can be done
		*  in constant time, and only invalidates iterators/references to
		*  the element being removed.
		*
		*  Note that no data is returned, and if the last element's data
		*  is needed, it should be retrieved before pop_back() is called.
		*/
		void pop_back()
		{ 
			erase(iterator(next_node(&m_base, m_base.m_prev))); 
		}

		inline void fix_next_link(node_base* prev, node_base* next)
		{
			node_base* oriPrev = next_node(next, next->m_prev);
			assert(oriPrev == prev);
			link(prev, next);
		}

	private:

		inline void link(node_base* prev, node_base* next)
		{
			prev->m_next = reinterpret_cast<char*>(next) - reinterpret_cast<char*>(prev);
			next->m_prev = -prev->m_next;
		}

		inline node_base* next_node(void* node, different_type offset)
		{
			return reinterpret_cast<node_base*>(reinterpret_cast<char*>(node) + offset);
		}

		inline pointer next_node(void* node, different_type offset) const
		{
			return reinterpret_cast<node_base*>(reinterpret_cast<char*>(node) + offset);
		}

		node_base m_base; // 这个节点的m_next指向begin,m_prev指向最后
						// 需要确保m_base地址和数组的偏移量不能发生变化
	};

	// 	_Tp必须存在成员变量m_prev用于表示前一个对象
	// 存在成员变量
	template <class _Tp>
	class stack_array_list
	{
	public:
		typedef array_list_node<_Tp>	value_type;
		typedef value_type&             reference;
		typedef const value_type&       const_reference;
		typedef value_type*             pointer;
		typedef const value_type*       const_pointer;
		typedef std::ptrdiff_t			different_type;
		stack_array_list()
		{
		}

		// 重新使用缓冲区方式创建之后调用
		void rebind_array_addr(pointer data)
		{
			m_base = data;
		}

		// 创建原始对象调用 
		void reset(pointer data, std::size_t count)
		{
			m_base = data;
			for (std::size_t i = 0; i < count - 1; ++i)
			{
				data[i].m_prev = i + 1;
				data[i].m_next = C_IDLE_NODE_FLAG;
			}
			data[count - 1].m_prev = C_TAIL_OF_STACK;
			data[count - 1].m_next = C_IDLE_NODE_FLAG;
			m_top = 0;
		}

		inline bool empty() const
		{
			return C_TAIL_OF_STACK == m_top;
		}

		/**
		*  Returns a read/write reference to the data at the last element
		*  of the %list.
		*/
		inline reference back()
		{ 
			return *(m_base + m_top);
		}

		/**
		*  Returns a read-only (constant) reference to the data at the last
		*  element of the %list.
		*/
		inline const_reference back() const
		{ 
			return *(m_base + m_top);
		}

		// node 必须是数组中的一位
		inline void push_back(reference node)
		{
			AssertOrphanNode(node);
			assert(!is_in_stack(node));
			node.m_prev = m_top;
			node.m_next = C_IDLE_NODE_FLAG;
			m_top = &node - m_base;
		}

		/**
		*  @brief  Removes last element.
		*
		*  This is a typical stack operation.  It shrinks the %list by
		*  one.  Due to the nature of a %list this operation can be done
		*  in constant time, and only invalidates iterators/references to
		*  the element being removed.
		*
		*  Note that no data is returned, and if the last element's data
		*  is needed, it should be retrieved before pop_back() is called.
		*/
		void pop_back()
		{ 
			m_top = (m_base + m_top)->m_prev;
		}

		inline bool is_in_stack(const_reference node) const 
		{
			return C_IDLE_NODE_FLAG == node.m_next;
		}

            pointer get_base(void)
            {
                return m_base;
            }

            different_type get_top(void)
            {
                return m_top;
            }
            
	private:
		static const int C_IDLE_NODE_FLAG = 0;
		static const int C_TAIL_OF_STACK = -1;
		pointer m_base; // 这个节点的m_next指向begin,m_prev指向最后
						// 需要确保m_base地址和数组的偏移量不能发生变化
		different_type m_top;	// 最上面对应的相对于m_base的偏移量
	};

}/* jy_LIB_ARRAY_LINKED_LIST_HEADER_ */



