#pragma once
#include <stdint.h>

class WinException
{
public:
	WinException(uint32_t error);

	wchar_t const* GetText() const;

private:
	static constexpr size_t m_size = 128;
	wchar_t m_buffer[m_size];
	uint32_t m_error;
};

class LastError : public WinException
{
public:
	LastError();
};

