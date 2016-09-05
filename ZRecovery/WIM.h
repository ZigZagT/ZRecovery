#pragma once
#include "stdafx.h"

class WIM_ImageInfo {
public:
	WIM_ImageInfo(std::wstring xml);
};

class WIM
{
public:
	WIM();
	WIM(std::wstring filepath);
	virtual ~WIM();
	
	void open(std::wstring filepath);
	size_t size();
	void set_temporary_path(std::wstring path);
	void capture(std::wstring path);
	void apply(size_t index, std::wstring path);
	WIM_ImageInfo get_info(size_t index);
	void set_info(size_t index, WIM_ImageInfo info);

	static std::wstring open_wim_file();
	static std::wstring save_wim_file();
	static bool test_file_exist(std::wstring path);

private:
	HANDLE _handle = NULL;
	bool _is_valid = false;
	bool _is_create_new = false;
};

