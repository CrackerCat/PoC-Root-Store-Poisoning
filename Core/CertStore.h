#pragma once
#include "CertContext.h"

#include <Neat\Types.h>
#include <Neat\Handle.h>

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

private:
	struct Traits
	{
		static bool IsValid(HCERTSTORE handle);
		static void Finalize(HCERTSTORE handle);
	};
	Neat::HandleT<HCERTSTORE, Traits> m_handle;
};

