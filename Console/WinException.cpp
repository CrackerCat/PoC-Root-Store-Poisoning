#include "stdafx.h"
#include "WinException.h"
#include <Windows.h>

WinException::WinException(uint32_t error) :
	m_error(error)
{
	::FormatMessageW(
		FORMAT_MESSAGE_FROM_SYSTEM,
		nullptr,
		m_error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		m_buffer,
		m_size,
		nullptr);
}

wchar_t const* WinException::GetText() const
{
	return m_buffer;
}

LastError::LastError() :
	WinException(::GetLastError())
{
}

