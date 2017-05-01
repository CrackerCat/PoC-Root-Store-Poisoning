#include "stdafx.h"
#include "CertContext.h"
#include "CertStore.h"
#include "CryptContext.h"
#include "WinException.h"

#include <exception>
#include <iostream>

int main(int argc, const char* argv[])
{
	try
	{
		const auto containerName = L"PoC";
		const auto certSubject = LR"(CN="Go Daddy Root Certificate Authority - G2", O="GoDaddy.com, Inc.", L=Scottsdale, S=Arizona, C=US, T=Test)";
		const auto validMinutes = 10;
		const auto storeName = L"Root";

		std::cout << "Creating container..." << std::endl;
		CryptContext context(containerName);

		std::cout << "Generating key..." << std::endl;
		auto key = context.GenerateKey();

		std::cout << "Generating certificate..." << std::endl;
		auto cert = CertContext::Create(containerName, certSubject, validMinutes);

		std::cout << "Importing certificate..." << std::endl;
		auto store = CertStore::Open(storeName);
		store.Import(cert);

		std::cout << "Success!" << std::endl;
	}
	catch (const std::exception& ex)
	{
		std::cout << "Error: " << ex.what() << std::endl;
	}
	catch (const WinException& ex)
	{
		std::wcout << L"Error: " << ex.GetText();
	}
	return 0;
}

