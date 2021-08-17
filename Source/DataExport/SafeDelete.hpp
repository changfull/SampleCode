#pragma once

template<class T> inline void SafeDelete(T*& p)
{
	// 전방 선언만 되어있을 경우 소멸자를 호출하지 않는 오류 방지
	typedef char type[sizeof(T) ? 1 : -1];
	// type의 컴파일 최적화 방지
	(void) sizeof(type);
	if (nullptr != p)
	{
		delete p;
		p = nullptr;
	}
}

template<class T> inline void SafeArrayDelete(T*& p)
{
	// 전방 선언만 되어있을 경우 소멸자를 호출하지 않는 오류 방지
	typedef char type[sizeof(T) ? 1 : -1];
	// type의 컴파일 최적화 방지
	(void) sizeof(type);
	if (nullptr != p)
	{
		delete[] p;
		p = nullptr;
	}
}