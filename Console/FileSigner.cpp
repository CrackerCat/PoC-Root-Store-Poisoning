#include "stdafx.h"
#include "FileSigner.h"
#include "WinException.h"
#include "WinFunction.h"
#include "WinHandle.h"

#include <Windows.h>
#include <wincrypt.h>

#include <exception>

//
// Missing data structures 
//

enum
{
	SIGNER_SUBJECT_FILE = 1,
	SIGNER_SUBJECT_BLOB = 2
};

enum
{
	SIGNER_CERT_SPC_FILE = 1,
	SIGNER_CERT_STORE = 2,
	SIGNER_CERT_SPC_CHAIN = 3
};

enum
{
	SIGNER_CERT_POLICY_STORE = 0x1,
	SIGNER_CERT_POLICY_CHAIN = 0x2,
	SIGNER_CERT_POLICY_CHAIN_NO_ROOT = 0x8
};

enum
{
	SIGNER_NO_ATTR = 0,
	SIGNER_AUTHCODE_ATTR = 1
};

enum
{
	PVK_TYPE_FILE_NAME = 1,
	PVK_TYPE_KEYCONTAINER = 2
};

typedef struct _SIGNER_FILE_INFO
{
	DWORD cbSize;
	LPCWSTR pwszFileName;
	HANDLE hFile;
} SIGNER_FILE_INFO, *PSIGNER_FILE_INFO;

typedef struct _SIGNER_BLOB_INFO
{
	DWORD cbSize;
	GUID *pGuidSubject;
	DWORD cbBlob;
	BYTE *pbBlob;
	LPCWSTR pwszDisplayName;
} SIGNER_BLOB_INFO, *PSIGNER_BLOB_INFO;

typedef struct _SIGNER_SUBJECT_INFO
{
	DWORD cbSize;
	DWORD *pdwIndex;
	DWORD dwSubjectChoice;
	union
	{
		SIGNER_FILE_INFO *pSignerFileInfo;
		SIGNER_BLOB_INFO *pSignerBlobInfo;
	};
} SIGNER_SUBJECT_INFO, *PSIGNER_SUBJECT_INFO;

typedef struct _SIGNER_CERT_STORE_INFO
{
	DWORD cbSize;
	PCCERT_CONTEXT pSigningCert;
	DWORD dwCertPolicy;
	HCERTSTORE hCertStore;
} SIGNER_CERT_STORE_INFO, *PSIGNER_CERT_STORE_INFO;

typedef struct _SIGNER_SPC_CHAIN_INFO
{
	DWORD cbSize;
	LPCWSTR pwszSpcFile;
	DWORD dwCertPolicy;
	HCERTSTORE hCertStore;
} SIGNER_SPC_CHAIN_INFO, *PSIGNER_SPC_CHAIN_INFO;

typedef struct _SIGNER_CERT
{
	DWORD cbSize;
	DWORD dwCertChoice;
	union
	{
		LPCWSTR pwszSpcFile;
		SIGNER_CERT_STORE_INFO *pCertStoreInfo;
		SIGNER_SPC_CHAIN_INFO *pSpcChainInfo;
	};
	HWND hwnd;
} SIGNER_CERT, *PSIGNER_CERT;

typedef struct _SIGNER_ATTR_AUTHCODE
{
	DWORD cbSize;
	BOOL fCommercial;
	BOOL fIndividual;
	LPCWSTR pwszName;
	LPCWSTR pwszInfo;
} SIGNER_ATTR_AUTHCODE, *PSIGNER_ATTR_AUTHCODE;

typedef struct _SIGNER_SIGNATURE_INFO
{
	DWORD cbSize;
	ALG_ID algidHash;
	DWORD dwAttrChoice;
	union
	{
		SIGNER_ATTR_AUTHCODE *pAttrAuthcode;
	};
	PCRYPT_ATTRIBUTES psAuthenticated;
	PCRYPT_ATTRIBUTES psUnauthenticated;
} SIGNER_SIGNATURE_INFO, *PSIGNER_SIGNATURE_INFO;

typedef struct _SIGNER_PROVIDER_INFO
{
	DWORD cbSize;
	LPCWSTR pwszProviderName;
	DWORD dwProviderType;
	DWORD dwKeySpec;
	DWORD dwPvkChoice;
	union
	{
		LPWSTR pwszPvkFileName;
		LPWSTR pwszKeyContainer;
	};
} SIGNER_PROVIDER_INFO, *PSIGNER_PROVIDER_INFO;

typedef struct _SIGNER_CONTEXT
{
	DWORD cbSize;
	DWORD cbBlob;
	BYTE *pbBlob;
} SIGNER_CONTEXT, *PSIGNER_CONTEXT;

//
// Missing functions
//

typedef HRESULT(WINAPI* SignerFreeSignerContext)(
	__in  SIGNER_CONTEXT *pSignerContext
);

typedef HRESULT(WINAPI *SignerSignEx)(
	__in      DWORD dwFlags,
	__in      SIGNER_SUBJECT_INFO *pSubjectInfo,
	__in      SIGNER_CERT *pSignerCert,
	__in      SIGNER_SIGNATURE_INFO *pSignatureInfo,
	__in_opt  SIGNER_PROVIDER_INFO *pProviderInfo,
	__in_opt  LPCWSTR pwszHttpTimeStamp,
	__in_opt  PCRYPT_ATTRIBUTES psRequest,
	__in_opt  LPVOID pSipData,
	__out     SIGNER_CONTEXT **ppSignerContext
);

//
// File signer code
//

void FileSigner::Sign(
	const wchar_t* filePath,
	const wchar_t* containerName,
	const wchar_t* timeStampServer,
	const CertContext& cert)
{
	auto signerSignEx = WinFunction<SignerSignEx>("Mssign32.dll", "SignerSignEx");
	if (!signerSignEx)
		throw std::runtime_error("SignerSignEx is not found in Mssign32.dll");

	auto signerFreeSignerContext = WinFunction<SignerFreeSignerContext>("Mssign32.dll", "SignerFreeSignerContext");
	if (!signerFreeSignerContext)
		throw std::runtime_error("SignerFreeSignerContext is not found in Mssign32.dll");

	WinHandle<> file = ::CreateFileW(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	
	SIGNER_FILE_INFO fileInfo = {};
	fileInfo.cbSize = sizeof(fileInfo);
	fileInfo.pwszFileName = filePath;
	fileInfo.hFile = file; // TODO: should be pass nullptr here and let Windows open file by itself?

	DWORD index = 0;

	SIGNER_SUBJECT_INFO subjectInfo = {};
	subjectInfo.cbSize = sizeof(subjectInfo);
	subjectInfo.pdwIndex = &index;
	subjectInfo.dwSubjectChoice = SIGNER_SUBJECT_FILE;
	subjectInfo.pSignerFileInfo = &fileInfo;

	SIGNER_CERT_STORE_INFO storeInfo = {};
	storeInfo.cbSize = sizeof(storeInfo);
	storeInfo.pSigningCert = cert.m_handle;
	storeInfo.dwCertPolicy = SIGNER_CERT_POLICY_CHAIN;

	SIGNER_CERT certInfo = {};
	certInfo.cbSize = sizeof(certInfo);
	certInfo.dwCertChoice = SIGNER_CERT_STORE;
	certInfo.pCertStoreInfo = &storeInfo;

	SIGNER_SIGNATURE_INFO signInfo = {};
	signInfo.cbSize = sizeof(signInfo);
	signInfo.algidHash = CALG_SHA1;

	SIGNER_PROVIDER_INFO providerInfo = {};
	providerInfo.cbSize = sizeof(providerInfo);
	providerInfo.dwProviderType = PROV_RSA_FULL;
	providerInfo.dwKeySpec = AT_SIGNATURE;
	providerInfo.dwPvkChoice = PVK_TYPE_KEYCONTAINER;
	providerInfo.pwszKeyContainer = const_cast<LPWSTR>(containerName);

	PSIGNER_CONTEXT signerContext = {};

	auto hr = signerSignEx(
		0,
		&subjectInfo,
		&certInfo,
		&signInfo,
		&providerInfo,
		timeStampServer,
		nullptr,
		nullptr,
		&signerContext);

	if (FAILED(hr))
		throw WinException(hr);

	signerFreeSignerContext(signerContext);
}