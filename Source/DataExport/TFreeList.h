#pragma once
#include <concurrent_queue.h>
#include <stack>
#include "Singleton.h"
#pragma push_macro("new")
#ifdef new
#undef new
#endif

#pragma push_macro("delete")
#ifdef delete
#undef delete
#endif

// TLS ��� ������� ����Ʈ
// 1. ���� ��� ����Ʈ�� concurrent_queue�� ����ؼ� ��Ƽ �����忡 �����ϴ�
// 2. ����ȭ ó���� ���� ���ϸ� ���̱� ���� TLS(thread local storage)�� ����Ѵ�.
// 3. TLS Ǯ�� ������Ʈ ����Ʈ�� ���� ��� �� ���� �Ҵ��ϰ� ������� ��� ���� Ǯ���� �Ҵ�.

template<typename T, int N = 1024>
class FreeList
{
public:
	FreeList()
	{
		// N ������ ��ŭ ���� ��ϸ���Ʈ ����
		for (auto i = 0; i < N; ++i)
		{
			_free_list.push(New());
		}
	}
	~FreeList() { FreeAll(); }

	__forceinline void FreeAll()
	{
		while ( !_free_list.empty())
		{
			T* data;
			if (_free_list.try_pop(data))
				Delete(data);
		}
	}

	// 1. ����Ʈ�� ���� �� pop queue
	// 2. ����Ʈ�� ���� �� new alloc
	__forceinline T* Alloc()
	{
		T* data;
		if (_free_list.try_pop(data))
			return data;
		else	
			return New();
	}

	// 1. push queue
	__forceinline void Free(T* p)
	{
		_free_list.push(p);
	}

protected:
	__forceinline T* New()
	{
#ifdef _DEBUG
		return reinterpret_cast<T*>(::operator new(sizeof(T), _NORMAL_BLOCK, __FILE__, __LINE__));
#else
		return reinterpret_cast<T*>(::operator new(sizeof(T)));
#endif
	}

	__forceinline void Delete(T* memory)
	{
#ifdef _DEBUG
		::operator delete(memory, _NORMAL_BLOCK, __FILE__, __LINE__);
#else
		::operator delete(memory);
#endif
	}

protected:
	Concurrency::concurrent_queue<T*> _free_list;
};

template<typename T, int N>
class TlsFreeList
{
protected:
	typename typedef Singleton<FreeList<T, N>> FreeListManager;
public:
	TlsFreeList() {}
	~TlsFreeList()
	{
		// ������ ����ÿ� �ٽ� ���� ���� ��ϸ���Ʈ�� push
		while ( ! _stack.empty())
		{
			T* object = _stack.top();
			_stack.pop();
			FreeListManager::GetInstance()->Free(reinterpret_cast<T*>(object));
		}
	}

	std::stack<T*> _stack;
};

template<typename T, int N = 1024>
class ObjectPool
{
protected:
	typename typedef Singleton<FreeList<T, N>> FreeListManager;

public:
	ObjectPool() {}
	virtual ~ObjectPool() {
	}

	__forceinline static void* operator new(std::size_t size)
	{
		// 1. tls�� ��������� ���� ���� ��ϸ���Ʈ���� �Ҵ�
		// 2. tls�� ������ pop
		if (_tls_free_list._stack.empty())
			return FreeListManager::GetInstance()->Alloc();
		else
		{
			T* memory = _tls_free_list._stack.top();
			_tls_free_list._stack.pop();
			return memory;
		}
	}

	__forceinline static void operator delete(void* memory, std::size_t /*size*/)
	{
		_tls_free_list._stack.push(reinterpret_cast<T*>(memory));
	}

private:
	thread_local static TlsFreeList<T, N> _tls_free_list;
};

template<typename T, int N>
thread_local TlsFreeList<T, N> ObjectPool<T, N>::_tls_free_list;

#pragma pop_macro("delete")
#pragma pop_macro("new")