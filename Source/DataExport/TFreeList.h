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

// TLS 기반 자유목록 리스트
// 1. 자유 목록 리스트는 concurrent_queue를 사용해서 멀티 쓰레드에 안전하다
// 2. 동기화 처리에 대한 부하를 줄이기 위해 TLS(thread local storage)를 사용한다.
// 3. TLS 풀에 오브젝트 리스트가 있을 경우 락 없이 할당하고 비어있을 경우 메인 풀에서 할당.

template<typename T, int N = 1024>
class FreeList
{
public:
	FreeList()
	{
		// N 사이즈 만큼 자유 목록리스트 생성
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

	// 1. 리스트에 있을 때 pop queue
	// 2. 리스트에 없을 때 new alloc
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
		// 쓰레드 종료시에 다시 메인 자유 목록리스트에 push
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
		// 1. tls가 비어있으면 메인 자유 목록리스트에서 할당
		// 2. tls가 있으면 pop
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