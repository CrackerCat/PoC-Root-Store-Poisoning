#include "stdafx.h"
#include "CertContext.h"
#include "WinException.h"

#include <vector>

#include <wincrypt.h>
#pragma comment(lib, "Crypt32.lib")

CertContext::CertContext()
{
}

CertContext CertContext::Create(
	const wchar_t* containerName,
	const wchar_t* certSubject,
	uint16_t validMinutes)
{
	auto encodeString = [](const wchar_t* string)
	{
		DWORD size = 0;
		auto success = ::CertStrToNameW(X509_ASN_ENCODING, string, CERT_X500_NAME_STR, nullptr, nullptr, &size, nullptr);
		if (!success)
			throw LastError();

		std::vector<BYTE> buffer(size);
		success = ::CertStrToNameW(X509_ASN_ENCODING, string, CERT_X500_NAME_STR, nullptr, &buffer[0], &size, nullptr);
		if (!success)
			throw LastError();

		return buffer;
	};

	auto subject = encodeString(certSubject);

	CERT_NAME_BLOB blob = {};
	blob.cbData = static_cast<DWORD>(subject.size());
	blob.pbData = &subject[0];

	CRYPT_KEY_PROV_INFO info = {};
	info.pwszContainerName = const_cast<LPWSTR>(containerName);
	info.dwProvType = PROV_RSA_FULL;
	info.dwFlags = CRYPT_MACHINE_KEYSET;
	info.dwKeySpec = AT_SIGNATURE;

	CRYPT_ALGORITHM_IDENTIFIER algo = {};
	algo.pszObjId = szOID_RSA_SHA1RSA;

	SYSTEMTIME endTime = {};
	::GetSystemTime(&endTime);
	endTime.wMinute += validMinutes;
	if (endTime.wMinute > 59)
	{
		endTime.wMinute -= 60;
		endTime.wHour += 1;
	}
	if (endTime.wHour > 23)
	{
		endTime.wHour -= 24;
		endTime.wDay += 1;
	}
	if (endTime.wDay > 31)
	{
		endTime.wDay -= 31;
		endTime.wMonth += 1;
	}
	if (endTime.wMonth > 12)
	{
		endTime.wMonth -= 12;
		endTime.wYear += 1;
	}

	CertContext context;
	context.m_handle = ::CertCreateSelfSignCertificate(0, &blob, 0, &info, &algo, nullptr, &endTime, nullptr);
	if (!context.m_handle)
		throw LastError();

	return context;
}

bool CertContext::IsValid(PCCERT_CONTEXT handle)
{
	return handle != nullptr;
}

void CertContext::Finalize(PCCERT_CONTEXT handle)
{
	::CertFreeCertificateContext(handle);
}


