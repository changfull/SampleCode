#pragma once
#include "Uncopyable.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN

class CriticalSection : private Uncopyable
{
public:
	CRITICAL_SECTION _cs;

	__forceinline CriticalSection()
	{
		InitializeCriticalSection(&_cs);
	}

	__forceinline ~CriticalSection()
	{
		DeleteCriticalSection(&_cs);
	}

	__forceinline void Enter()
	{
		EnterCriticalSection(&_cs);
	}

	__forceinline void Leave()
	{
		LeaveCriticalSection(&_cs);
	}

	class Lock
	{
	public:
		CriticalSection& _cs;

		__forceinline Lock(CriticalSection& cs) : _cs(cs)
		{
			_cs.Enter();
		}

		__forceinline  ~Lock(void)
		{
			_cs.Leave();
		}
	};
};



