#pragma once
#include <Windows.h>

struct DefaultWinHandleTraits
{
	static bool IsValid(HANDLE handle)
	{
		return handle != INVALID_HANDLE_VALUE;
	}

	static void Finalize(HANDLE handle)
	{
		::CloseHandle(handle);
	}
};

template <typename T = HANDLE, typename Traits = DefaultWinHandleTraits>
class WinHandle
{
public:
	WinHandle();
	WinHandle(T handle);
	WinHandle(WinHandle&& other);
	~WinHandle();

	bool IsNull() const;
	bool IsValid() const;

	void Close();
	void Attach(T handle);

	T Detach();
	operator T() const;

	T* operator&();
	const T* operator&() const;

	WinHandle& operator=(T handle);
	WinHandle& operator=(WinHandle&& other);

protected:
	T m_handle;
};

template <typename T, typename Traits>
WinHandle<T, Traits>::WinHandle()
{
	memset(&m_handle, 0, sizeof(m_handle));
}

template <typename T, typename Traits>
WinHandle<T, Traits>::WinHandle(T handle) :
	m_handle(handle)
{
}

template <typename T, typename Traits>
WinHandle<T, Traits>::WinHandle(WinHandle&& other) :
	m_handle(other.m_handle)
{
	memset(&other.m_handle, 0, sizeof(m_handle));
}

template <typename T, typename Traits>
WinHandle<T, Traits>::~WinHandle()
{
	Close();
}

template <typename T, typename Traits>
bool WinHandle<T, Traits>::IsNull() const
{
	static const WinHandle<T, Traits> s_null;
	if (0 == memcmp(&m_handle, &s_null.m_handle, sizeof(m_handle)))
		return true;

	return false;
}

template <typename T, typename Traits>
bool WinHandle<T, Traits>::IsValid() const
{
	return Traits::IsValid(m_handle);
}

template <typename T, typename Traits>
void WinHandle<T, Traits>::Close()
{
	if (!IsNull())
	{
		Traits::Finalize(m_handle);
		memset(&m_handle, 0, sizeof(m_handle));
	}
}

template <typename T, typename Traits>
void WinHandle<T, Traits>::Attach(T handle)
{
	Close();
	m_handle = handle;
}

template <typename T, typename Traits>
T WinHandle<T, Traits>::Detach()
{
	T handle = m_handle;
	memset(&m_handle, 0, sizeof(m_handle));
	return handle;
}

template <typename T, typename Traits>
WinHandle<T, Traits>::operator T() const
{
	return m_handle;
}

template <typename T, typename Traits>
T* WinHandle<T, Traits>::operator&()
{
	return &m_handle;
}

template <typename T, typename Traits>
const T* WinHandle<T, Traits>::operator&() const
{
	return &m_handle;
}

template <typename T, typename Traits>
WinHandle<T, Traits>& WinHandle<T, Traits>::operator=(T handle)
{
	Attach(handle);
	return *this;
}

template <typename T, typename Traits>
WinHandle<T, Traits>& WinHandle<T, Traits>::operator=(WinHandle&& other)
{
	Attach(other.Detach());
	return *this;
}