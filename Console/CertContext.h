#pragma once
#include "WinHandle.h"
#include <stdint.h>
#include <Windows.h>

class CertContext
{
	CertContext();

public:
	CertContext(CertContext&& other) = default;
	CertContext& operator=(CertContext&& other) = default;

	CertContext(const CertContext&) = delete;
	CertContext& operator=(const CertContext&) = delete;

	static CertContext Create(
		const wchar_t* containerName,
		const wchar_t* certSubject,
		uint16_t validMinutes);

	static bool IsValid(PCCERT_CONTEXT handle);
	static void Finalize(PCCERT_CONTEXT handle);

private:
	WinHandle<PCCERT_CONTEXT, CertContext> m_handle;

	friend class CertStore;
};

