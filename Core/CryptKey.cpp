#include "CryptKey.h"

#include <wincrypt.h>
#pragma comment(lib, "Crypt32.lib")

using namespace Neat;

CryptKey::CryptKey()
{
}

bool CryptKey::Traits::IsValid(HCRYPTKEY handle)
{
	return handle != NULL;
}

void CryptKey::Traits::Finalize(HCRYPTKEY handle)
{
	::CryptDestroyKey(handle);
}
