#pragma once
#include "CryptKey.h"

#include <Neat\Types.h>
#include <Neat\Handle.h>

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

private:
	struct Traits
	{
		static bool IsValid(HCRYPTPROV handle);
		static void Finalize(HCRYPTPROV handle);
	};
	Neat::HandleT<HCRYPTPROV, Traits> m_handle;
	std::wstring m_name;
};

