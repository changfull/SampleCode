#pragma once
#include <winbase.h>

inline void Verify(bool check) {
	DWORD last_error = ::GetLastError();
	UNREFERENCED_PARAMETER(last_error);
	if (!check)
		::__debugbreak();
}