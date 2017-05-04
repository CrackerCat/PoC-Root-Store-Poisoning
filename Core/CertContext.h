#pragma once
#include "WinHandle.h"
#include <stdint.h>
#include <string_view>
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
		std::wstring_view containerName,
		std::wstring_view certSubject,
		std::wstring_view friendlyName,
		uint16_t validMinutes);

	static bool IsValid(PCCERT_CONTEXT handle);
	static void Finalize(PCCERT_CONTEXT handle);

private:
	WinHandle<PCCERT_CONTEXT, CertContext> m_handle;

	friend class CertStore;
	friend class FileSigner;
};

