#include "stdafx.h"
#include "CryptContext.h"
#include "WinException.h"

#include <wincrypt.h>
#pragma comment(lib, "Crypt32.lib")

CryptContext::CryptContext(const wchar_t* name) :
	m_name(name)
{
	auto success = ::CryptAcquireContextW(&m_handle, name, nullptr, PROV_RSA_FULL, CRYPT_MACHINE_KEYSET);
	if (!success)
		success = ::CryptAcquireContextW(&m_handle, name, nullptr, PROV_RSA_FULL, CRYPT_NEWKEYSET | CRYPT_MACHINE_KEYSET);
	if (!success)
		throw LastError();
}

CryptKey CryptContext::GenerateKey()
{
	CryptKey key;
	auto success = ::CryptGenKey(m_handle, AT_SIGNATURE, 0x08000000/* RSA-2048-BIT_KEY */, &key.m_handle);
	if (!success)
		throw LastError();
	return key;
}

bool CryptContext::IsValid(HCRYPTPROV handle)
{
	return handle != NULL;
}

void CryptContext::Finalize(HCRYPTPROV handle)
{
	::CryptReleaseContext(handle, 0);
}

