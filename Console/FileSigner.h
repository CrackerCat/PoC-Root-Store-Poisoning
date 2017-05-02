#pragma once
#include "CertContext.h"

class FileSigner
{
public:
	static void Sign(
		const wchar_t* filePath,
		const wchar_t* containerName,
		const wchar_t* timeStampServer,
		const CertContext& cert);
};

