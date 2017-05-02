#pragma once
#include "CertContext.h"
#include "WinHandle.h"
#include <Windows.h>

class CertStore
{
	CertStore();

public:
	CertStore(CertStore&& other) = default;
	CertStore& operator=(CertStore&& other) = default;

	CertStore(const CertStore&) = delete;
	CertStore& operator=(const CertStore&) = delete;

	static CertStore Open(const wchar_t* name);

	void Import(const CertContext& cert);

	static bool IsValid(HCERTSTORE handle);
	static void Finalize(HCERTSTORE handle);

private:
	WinHandle<HCERTSTORE, CertStore> m_handle;
};

