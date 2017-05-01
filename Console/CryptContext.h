#pragma once
#include "CryptKey.h"
#include "WinHandle.h"
#include <string>
#include <Windows.h>

class CryptContext
{
public:
	CryptContext(const wchar_t* containerName);

	CryptContext(CryptContext&& other) = default;
	CryptContext& operator=(CryptContext&& other) = default;

	CryptContext(const CryptContext&) = delete;
	CryptContext& operator=(const CryptContext&) = delete;

	CryptKey GenerateKey();

	static bool IsValid(HCRYPTPROV handle);
	static void Finalize(HCRYPTPROV handle);

private:
	std::wstring m_name;
	WinHandle<HCRYPTPROV, CryptContext> m_handle;
};

