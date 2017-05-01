#pragma once
#include "WinHandle.h"
#include <Windows.h>

class CryptKey
{
	CryptKey();

public:
	CryptKey(CryptKey&& other) = default;
	CryptKey& operator=(CryptKey&& other) = default;

	CryptKey(const CryptKey&) = delete;
	CryptKey& operator=(const CryptKey&) = delete;

	static bool IsValid(HCRYPTKEY handle);
	static void Finalize(HCRYPTKEY handle);

private:
	WinHandle<HCRYPTKEY, CryptKey> m_handle;

	friend class CryptContext;
};

