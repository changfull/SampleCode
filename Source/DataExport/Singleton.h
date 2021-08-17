#pragma once
//#include <mutex>
#include "Lock.h"

template<class T>
class Singleton
{
private:
	Singleton(void) = default;
	Singleton(const Singleton &) = delete;
	Singleton<T>& operator=(const Singleton<T>& rhs) = delete;
	virtual ~Singleton(void)
	{
		_destroyed = true;
	}

public:
	static void Destroy()
	{
		_instance->~T();
	}

	static T* GetInstance()
	{	
		if (_destroyed)
		{
			new(_instance)T;	// ���� ��ü �Ҹ�� �ش� �޸𸮸� �ʱ�ȭ���� �ʱ� ������ �����ڸ� ȣ���ؼ� ��ü�� �ǻ츲
			atexit(Destroy);
			_destroyed = false;
		}
		else if (0 == _instance)
		{
			// DCL(Double Checked Locking)
			//std::lock_guard<std::mutex> lock(_mutex); 
			_cs.Enter();
			if (0 == _instance)
			{
				static T instance;
				_instance = &instance;
			}
			_cs.Leave();
		}

		return _instance;
	}

private:
	static bool _destroyed;
	static T* _instance;

	//static std::mutex _mutex;
	static CriticalSection _cs;
};

template<typename T>
bool Singleton<T>::_destroyed = false;

template<typename T>
T* Singleton<T>::_instance = nullptr;

//template<typename T>
//std::mutex Singleton<T>::_mutex;

template<typename T>
CriticalSection Singleton<T>::_cs;