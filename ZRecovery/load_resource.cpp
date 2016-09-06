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
		dict_entry(IDS_RESTORE),
		dict_entry(IDS_FACTORY),
		dict_entry(IDS_SELECT_WIM_PATH),
		dict_entry(IDS_BROWSE),
		dict_entry(IDS_WIM_VERIFY),
		dict_entry(IDS_WIM_COMPRESS),
		dict_entry(IDS_BACKUP_CREATE),
		dict_entry(IDS_BACKUP_APPEND),
		dict_entry(IDS_CREATE_BACKUP_NAME),
		dict_entry(IDS_CREATE_BACKUP_DESCRIPTION),
		dict_entry(IDS_CREATE_BACKUP),
		dict_entry(IDS_SELECT_BACKUP_FOR_RECOVERY),
		dict_entry(IDS_BACKUP_INDEX),
		dict_entry(IDS_BACKUP_NAME),
		dict_entry(IDS_BACKUP_DATE),
		dict_entry(IDS_BACKUP_DESCRIPTION),
		dict_entry(IDS_SELECT_BACKUP_TO_SEE_DESCRIPTION),
		dict_entry(IDS_RESTORE_TO_SELECT_BACKUP),
		dict_entry(IDS_DELETE_SELECTED_BACKUP),
		dict_entry(IDS_SELECT_FACTORY_BACKUP),
		dict_entry(IDS_CREATE_FACTORY_BACKUP),
		dict_entry(IDS_DELETE_SELECTED_FACTORY_BACKUP),
		dict_entry(IDS_RESTORE_TO_SELECT_FACTORY_BACKUP)
	};
	auto uid = dict.at(id);
	return load_resource<std::wstring>(hInst, uid);
#undef dict_entry
}