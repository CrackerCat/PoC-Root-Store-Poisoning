#include "CertStore.h"
#include <LibNeat\Neat\Win\Exception.h>

#include <wincrypt.h>
#pragma comment(lib, "Crypt32.lib")

using namespace Neat::Win;

CertStore::CertStore()
{
}

CertStore CertStore::Open(const wchar_t* name)
{
	CertStore store;
	store.m_handle = ::CertOpenStore(CERT_STORE_PROV_SYSTEM, 0, 0, CERT_SYSTEM_STORE_LOCAL_MACHINE, name);
	if (!store.m_handle)
		throw LastErrorException();
	return store;
}

void CertStore::Import(const CertContext& cert)
{
	auto success = ::CertAddCertificateContextToStore(m_handle, cert.m_handle, CERT_STORE_ADD_REPLACE_EXISTING, 0);
	if (!success)
		throw LastErrorException();
}

bool CertStore::Traits::IsValid(HCERTSTORE handle)
{
	return handle != nullptr;
}

void CertStore::Traits::Finalize(HCERTSTORE handle)
{
	::CertCloseStore(handle, 0);
}
