#include "stdafx.h"
#include "WinTime.h"

WinTime::WinTime()
{
	::GetSystemTime(&m_time);
}

void WinTime::MoveForward(uint16_t minutes)
{
	m_time.wMinute += minutes;
	if (m_time.wMinute > 59)
	{
		m_time.wMinute -= 60;
		m_time.wHour += 1;
	}
	if (m_time.wHour > 23)
	{
		m_time.wHour -= 24;
		m_time.wDay += 1;
	}
	if (m_time.wDay > 31)
	{
		m_time.wDay -= 31;
		m_time.wMonth += 1;
	}
	if (m_time.wMonth > 12)
	{
		m_time.wMonth -= 12;
		m_time.wYear += 1;
	}
}

void WinTime::MoveBackward(uint16_t years)
{
	m_time.wYear -= years;
}
