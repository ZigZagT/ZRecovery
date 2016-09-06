#pragma once
#include "stdafx.h"
#include "COM_Proxy.h"

class WIM_ImageInfo {
public:
	WIM_ImageInfo(std::wstring xml);
	~WIM_ImageInfo();

	std::wstring get_name();
	std::wstring get_description();
	std::wstring get_date();
	std::wstring get_date_localtime();

	void set_name(std::wstring name);
	void set_description(std::wstring description);
	void set_date(std::wstring date);
	void set_date_now();

	std::wstring to_xml();

private:
	COM_Proxy<IXMLDOMDocument> _doc;
	COM_Proxy<IXMLDOMNode> _name;
	COM_Proxy<IXMLDOMNode> _description;
	COM_Proxy<IXMLDOMNode> _date;
	static const wchar_t leading_code = 0xfeff;
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

