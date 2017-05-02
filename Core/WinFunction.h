#pragma once
#include <Windows.h>

template <class Fn>
Fn WinFunction(const char* module, const char* function)
{
	return (Fn)::GetProcAddress(::LoadLibraryA(module), function);
}
