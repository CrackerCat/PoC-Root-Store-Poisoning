#include "CertStore.h"
#include "WinException.h"

#include <wincrypt.h>
#pragma comment(lib, "Crypt32.lib")

CertStore::CertStore()
{
}

CertStore CertStore::Open(const wchar_t* name)
{
	CertStore store;
	store.m_handle = ::CertOpenStore(CERT_STORE_PROV_SYSTEM, 0, 0, CERT_SYSTEM_STORE_LOCAL_MACHINE, name);
	if (!store.m_handle)
		throw LastError();
	return store;
}

void CertStore::Import(const CertContext& cert)
{
	auto success = ::CertAddCertificateContextToStore(m_handle, cert.m_handle, CERT_STORE_ADD_REPLACE_EXISTING, 0);
	if (!success)
		throw LastError();
}

bool CertStore::IsValid(HCERTSTORE handle)
{
	return handle != nullptr;
}

void CertStore::Finalize(HCERTSTORE handle)
{
	::CertCloseStore(handle, 0);
}

