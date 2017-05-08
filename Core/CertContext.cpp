#include "CertContext.h"
#include <Neat\Win\Exception.h>
#include <Neat\Win\Time.h>

#include <vector>

#include <wincrypt.h>
#pragma comment(lib, "Crypt32.lib")

using namespace Neat::Win;

CertContext::CertContext()
{
}

CertContext CertContext::Create(
	std::wstring_view containerName,
	std::wstring_view certSubject,
	std::wstring_view friendlyName,
	uint16_t validMinutes)
{
	auto encodeString = [](const wchar_t* string)
	{
		DWORD size = 0;
		auto success = ::CertStrToNameW(X509_ASN_ENCODING, string, CERT_X500_NAME_STR, nullptr, nullptr, &size, nullptr);
		if (!success)
			throw LastErrorException();

		std::vector<BYTE> buffer(size);
		success = ::CertStrToNameW(X509_ASN_ENCODING, string, CERT_X500_NAME_STR, nullptr, &buffer[0], &size, nullptr);
		if (!success)
			throw LastErrorException();

		return buffer;
	};

	auto subject = encodeString(certSubject.data());

	CERT_NAME_BLOB blob = {};
	blob.cbData = static_cast<DWORD>(subject.size());
	blob.pbData = &subject[0];

	CRYPT_KEY_PROV_INFO info = {};
	info.pwszContainerName = const_cast<LPWSTR>(containerName.data());
	info.dwProvType = PROV_RSA_FULL;
	info.dwFlags = CRYPT_MACHINE_KEYSET;
	info.dwKeySpec = AT_SIGNATURE;

	CRYPT_ALGORITHM_IDENTIFIER algo = {};
	algo.pszObjId = szOID_RSA_SHA1RSA;

	Time startTime;
	startTime.MoveBackward(1);

	Time endTime;
	endTime.MoveForward(validMinutes);

	CertContext context;
	context.m_handle = ::CertCreateSelfSignCertificate(0, &blob, 0, &info, &algo, &startTime, &endTime, nullptr);
	if (!context.m_handle)
		throw LastErrorException();

	//CRYPT_DATA_BLOB data = {};
	//data.cbData = (friendlyName.size() + 1) * sizeof(wchar_t);
	//data.pbData = (PBYTE)friendlyName.data();

	//auto success = ::CertSetCertificateContextProperty(context.m_handle, CERT_FRIENDLY_NAME_PROP_ID, 0, &data);
	//if (!success)
	//	throw LastError();

	return context;
}

bool CertContext::Traits::IsValid(PCCERT_CONTEXT handle)
{
	return handle != nullptr;
}

void CertContext::Traits::Finalize(PCCERT_CONTEXT handle)
{
	::CertFreeCertificateContext(handle);
}
