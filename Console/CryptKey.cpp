#include "stdafx.h"
#include "CryptKey.h"

#include <wincrypt.h>
#pragma comment(lib, "Crypt32.lib")

CryptKey::CryptKey()
{
}

bool CryptKey::IsValid(HCRYPTKEY handle)
{
	return handle != NULL;
}

void CryptKey::Finalize(HCRYPTKEY handle)
{
	::CryptDestroyKey(handle);
}

