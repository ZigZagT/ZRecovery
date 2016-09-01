#pragma once
#include "stdafx.h"

inline std::string trim_right_copy(
	const std::string& s,
	const std::string& delimiters = " \f\n\r\t\v")
{
	auto pos = s.find_last_not_of(delimiters);
	if (pos == s.npos) {
		return "";
	}
	else {
		return s.substr(0, pos + 1);
	}
}

inline std::wstring trim_right_copy(
	const std::wstring& s,
	const std::wstring& delimiters = L" \f\n\r\t\v")
{
	auto pos = s.find_last_not_of(delimiters);
	if (pos == s.npos) {
		return L"";
	}
	else {
		return s.substr(0, pos + 1);
	}
}

inline std::string trim_left_copy(
	const std::string& s,
	const std::string& delimiters = " \f\n\r\t\v")
{
	auto pos = s.find_first_not_of(delimiters);
	if (pos == s.npos) {
		return "";
	}
	else {
		return s.substr(pos);
	}
}

inline std::wstring trim_left_copy(
	const std::wstring& s,
	const std::wstring& delimiters = L" \f\n\r\t\v")
{
	auto pos = s.find_first_not_of(delimiters);
	if (pos == s.npos) {
		return L"";
	}
	else {
		return s.substr(pos);
	}
}

inline std::string trim_copy(
	const std::string& s,
	const std::string& delimiters = " \f\n\r\t\v")
{
	return trim_left_copy(trim_right_copy(s, delimiters), delimiters);
}

inline std::wstring trim_copy(
	const std::wstring& s,
	const std::wstring& delimiters = L" \f\n\r\t\v")
{
	return trim_left_copy(trim_right_copy(s, delimiters), delimiters);
}

inline bool case_insensitive_compare(std::string s1, std::string s2) {
	return _stricmp(s1.c_str(), s2.c_str()) == 0;
	//if (s1.length() == s2.length()) {
	//	return std::equal(s1.begin(), s1.end(),
	//		s2.begin(), [](char a, char b) {
	//		std::locale loc;
	//		return std::tolower(a, loc) == std::tolower(b, loc);
	//	});
	//}
	//else {
	//	return false;
	//}
}