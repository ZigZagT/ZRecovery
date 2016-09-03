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

template<>
std::wstring load_resource<std::wstring>(HINSTANCE hInst, std::wstring id) {
#define dict_entry(resource_id) {L#resource_id, resource_id}
	static std::map<std::wstring, int> dict = {
		dict_entry(IDS_APP_TITLE),
		dict_entry(IDS_WIM_FILE),
		dict_entry(IDS_BACKUP),
		dict_entry(IDS_RECOVERY),
		dict_entry(IDS_FACTORY),
		dict_entry(IDS_SELECT_WIM_PATH),
		dict_entry(IDS_BROWSE)
	};
	auto uid = dict.at(id);
	return load_resource<std::wstring>(hInst, uid);
#undef dict_entry
}