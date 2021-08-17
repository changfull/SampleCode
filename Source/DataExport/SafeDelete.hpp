#pragma once

template<class T> inline void SafeDelete(T*& p)
{
	// ���� ���� �Ǿ����� ��� �Ҹ��ڸ� ȣ������ �ʴ� ���� ����
	typedef char type[sizeof(T) ? 1 : -1];
	// type�� ������ ����ȭ ����
	(void) sizeof(type);
	if (nullptr != p)
	{
		delete p;
		p = nullptr;
	}
}

template<class T> inline void SafeArrayDelete(T*& p)
{
	// ���� ���� �Ǿ����� ��� �Ҹ��ڸ� ȣ������ �ʴ� ���� ����
	typedef char type[sizeof(T) ? 1 : -1];
	// type�� ������ ����ȭ ����
	(void) sizeof(type);
	if (nullptr != p)
	{
		delete[] p;
		p = nullptr;
	}
}