#include "stdafx.h"
#include "load_resource.h"
#include "Debug.h"

template<>
std::wstring load_resource<std::wstring>(HINSTANCE hInst, UINT uID)
{
	static const size_t max_len = 100;
	static wchar_t str[max_len];
	auto len = LoadStringW(hInst, uID, str, max_len);
	return std::wstring(str, len);
}