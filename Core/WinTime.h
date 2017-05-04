#pragma once
#include <Windows.h>

class WinTime
{
public:
	WinTime();

	void MoveForward(uint16_t minutes);
	void MoveBackward(uint16_t years);

	SYSTEMTIME* operator&();
	const SYSTEMTIME* operator&() const;

private:
	SYSTEMTIME m_time;
};

inline SYSTEMTIME* WinTime::operator&()
{
	return &m_time;
}

inline const SYSTEMTIME* WinTime::operator&() const
{
	return &m_time;
}

