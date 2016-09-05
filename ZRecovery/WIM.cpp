#include "stdafx.h"
#include "WIM.h"
#include "ZRecovery.h"
#include "load_resource.h"
#include "debug.h"


WIM::WIM()
{
}

WIM::WIM(std::wstring filepath)
{
	open(filepath);
}

WIM::~WIM()
{
	if (_is_valid) {
		WIMCloseHandle(_handle);
	}
}

void WIM::open(std::wstring filepath)
{
	if (_is_valid) {
		WIMCloseHandle(_handle);
		_handle = NULL;
		_is_valid = false;
		_is_create_new = false;
	}

	DWORD is_create;
	_handle = WIMCreateFile(filepath.c_str(), WIM_GENERIC_READ | WIM_GENERIC_WRITE, WIM_OPEN_ALWAYS, WIM_FLAG_VERIFY, WIM_COMPRESS_LZX, &is_create);
	if (_handle != NULL) {
		_is_valid = true;
		if (is_create == WIM_CREATED_NEW) {
			_is_create_new = true;
		}
	}
	else {
		auto k = GetLastError();
		throw std::runtime_error("WIM::open failed");
	}
}

size_t WIM::size()
{
	if (_is_valid) {
		return WIMGetImageCount(_handle);
	}
	throw std::runtime_error("invalid handle");
}

void WIM::set_temporary_path(std::wstring path)
{
	if (WIMSetTemporaryPath(_handle, path.c_str()) == 0) {
		throw std::runtime_error("set_temporary_path failed");
	}
}

void WIM::capture(std::wstring path)
{
	auto handle = WIMCaptureImage(_handle, path.c_str(), WIM_FLAG_VERIFY);
	if (handle == NULL) {
		throw std::runtime_error("capture image failed");
	}
	WIMCloseHandle(handle);
}

void WIM::apply(size_t index, std::wstring path)
{
	auto handle = WIMLoadImage(_handle, index);
	if (handle == NULL) {
		throw std::runtime_error("WIMLoadImage failed");
	}
	auto res = WIMApplyImage(handle, path.c_str(), WIM_FLAG_VERIFY);
	WIMCloseHandle(handle);
	if (res == 0) {
		throw std::runtime_error("Apply image failed");
	}
}

WIM_ImageInfo WIM::get_info(size_t index)
{
	if (!_is_valid) {
		throw std::runtime_error("invalid handle");
	}
	HANDLE h = WIMLoadImage(_handle, index + 1);
	if (h == NULL) {
		auto k = GetLastError();
		throw std::runtime_error("load image filed");
	}
	std::shared_ptr<void> handle(h, [](auto p) { WIMCloseHandle(p); });

	void* info;
	DWORD size;

	auto res = WIMGetImageInformation(handle.get(), &info, &size);
	std::shared_ptr<void> information(info, [&size](auto p) {LocalFree(p); });

	if (res == 0) {
		throw std::runtime_error("get information failed");
	}

	std::wstring xml(static_cast<wchar_t*>(info), size);
	//Alert(xml);
	return WIM_ImageInfo(xml);
}

void WIM::set_info(size_t index, WIM_ImageInfo info)
{
	if (!_is_valid) {
		throw std::runtime_error("invalid handle");
	}
	HANDLE h = WIMLoadImage(_handle, index);
	if (h == NULL) {
		throw std::runtime_error("load image filed");
	}
	std::shared_ptr<void> handle(h, [](auto p) { WIMCloseHandle(p); });

	auto xml = info.to_xml();
	auto res = WIMSetImageInformation(h, reinterpret_cast<void*>(const_cast<wchar_t*>(xml.c_str())), xml.length());

	if (res == 0) {
		throw std::runtime_error("set information failed");
	}
}



std::wstring WIM::open_wim_file()
{
	extern HINSTANCE hInst;
	std::shared_ptr<IFileOpenDialog> pFileOpen;
	IFileOpenDialog *tempFileOpen;
	auto hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&tempFileOpen));
	if (FAILED(hr)) {
		throw std::runtime_error("Create FileOpenDialog failed");
	}
	else {
		pFileOpen.reset(tempFileOpen, [](auto p) { p->Release(); });
	}

	auto str = load_resource < std::wstring >(hInst, IDS_WIM_FILE);
	COMDLG_FILTERSPEC available_file_types[] = {
		{ str.c_str(), L"*.wim" },
		{ L"Any", L"*.*" }
	};
	pFileOpen->SetFileTypes(sizeof(available_file_types) / sizeof(COMDLG_FILTERSPEC), available_file_types);

	//FILEOPENDIALOGOPTIONS opt;
	//pFileOpen->GetOptions(&opt);
	//pFileOpen->SetOptions(opt);
	hr = pFileOpen->Show(NULL);
	if (FAILED(hr))
	{
		return L"";
	}
	IShellItem* item;
	hr = pFileOpen->GetResult(&item);
	if (FAILED(hr))
	{
		throw std::runtime_error("GetResult failed");
	}
	std::shared_ptr<wchar_t> file;
	LPWSTR tempfile;
	item->GetDisplayName(SIGDN_FILESYSPATH, &tempfile);
	file.reset(tempfile, [](auto p) {CoTaskMemFree(p); });

	std::wstring ret;
	//Alert(file.get());
	ret = std::wstring(file.get());
	return ret;
}

std::wstring WIM::save_wim_file()
{
	extern HINSTANCE hInst;
	std::shared_ptr<IFileSaveDialog> pFileSave;
	IFileSaveDialog *temp;
	auto hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&temp));
	if (FAILED(hr)) {
		throw std::runtime_error("Create FileSaveDialog failed");
	}
	else {
		pFileSave.reset(temp, [](auto p) { p->Release(); });
	}

	auto str = load_resource < std::wstring >(hInst, IDS_WIM_FILE);
	COMDLG_FILTERSPEC available_file_types[] = {
		{ str.c_str(), L"*.wim" },
		{ L"Any", L"*.*" }
	};
	pFileSave->SetFileTypes(sizeof(available_file_types) / sizeof(COMDLG_FILTERSPEC), available_file_types);

	//FILEOPENDIALOGOPTIONS opt;
	//pFileSave->GetOptions(&opt);
	//pFileSave->SetOptions(opt);
	hr = pFileSave->Show(NULL);
	if (FAILED(hr))
	{
		return L"";
	}
	IShellItem* item;
	hr = pFileSave->GetResult(&item);
	if (FAILED(hr))
	{
		throw std::runtime_error("GetResult failed");
	}
	std::shared_ptr<wchar_t> file;
	LPWSTR tempfile;
	item->GetDisplayName(SIGDN_FILESYSPATH, &tempfile);
	file.reset(tempfile, [](auto p) {CoTaskMemFree(p); });

	std::wstring ret;
	Alert(file.get());
	ret = std::wstring(file.get());
	return ret;
}

bool WIM::test_file_exist(std::wstring path)
{
	if (path.size() < 1) {
		return false;
	}
	bool ret = false;
	auto wim = WIMCreateFile(path.c_str(), 0, WIM_OPEN_EXISTING, 0, 0, NULL);
	if (wim != NULL) {
		ret = true;
		WIMCloseHandle(wim);
	}
	return ret;
}

WIM_ImageInfo::WIM_ImageInfo(std::wstring xml)
{
	if (xml[0] == leading_code) {
		xml[0] = ' ';
	}

#define test_hr(msg) if (hr != S_OK) { k = GetLastError();	throw std::runtime_error(msg);	}
	auto hr = CoCreateInstance(CLSID_DOMDocument60, nullptr, CLSCTX_ALL, IID_IXMLDOMDocument2, reinterpret_cast<void**>(&_doc));
	decltype(GetLastError()) k;
	if (FAILED(hr)) {
		throw std::runtime_error("create IXMLDocument failed");
	}
	VARIANT_BOOL res;
	hr = _doc->loadXML(const_cast<BSTR>(xml.c_str()), &res);
	test_hr("loadXML failed");
	
	std::wstringstream oss;
	BSTR str;

	//IXMLDOMParseError *err;
	//_doc->get_parseError(&err);
	//err->get_reason(&str);
	//oss << "error: " << str;
	//long code;
	//err->get_errorCode(&code);
	//oss << "error code: " << code << std::endl;
	//long line;
	//err->get_line(&line);
	//oss << "at line " << line << std::endl;
	//long pos;
	//err->get_linepos(&pos);
	//oss << "pos " << pos << std::endl;
	//err->get_srcText(&str);
	//oss << "with source: \n" << str << std::endl;

	//IXMLDOMNodeList *children;
	//hr = _doc->get_childNodes(&children);
	//
	//long length;
	//hr = children->get_length(&length);
	//test_hr(get length failed);
	//oss << "children count: " << length << std::endl;

	//hr = node->get_nodeName(&str);
	//oss << "node.nodeName: " << std::wstring(str) << std::endl;
	//test_hr(get nodeName failed);

	//hr = node->get_baseName(&str);
	//oss << "node.baseName: " << std::wstring(str) << std::endl;
	//test_hr(get baseName failed);

	// Name
	hr = _doc->selectSingleNode(L"/IMAGE/NAME", &_name);
	if (hr == S_FALSE) {
		VARIANT val;
		val.vt = VT_I4;
		val.intVal = NODE_ELEMENT;
		hr = _doc->createNode(val, L"NAME", NULL, &_name);
		test_hr("create node failed");
		IXMLDOMNode* out;
		hr = _doc->appendChild(_name, &out);
		test_hr("append child failed");
		if (out != NULL) {
			_name->Release();
			_name = out;
		}
	}
	test_hr("select node /IMAGE/NAME failed");
	hr = _name->hasChildNodes(&res);
	if (!res) {
		IXMLDOMText* text;
		IXMLDOMNode* insert_text;
		_doc->createTextNode(L" ", &text);
		_name->appendChild(text, &insert_text);
		text->Release();
		insert_text->Release();
	}

	// Description
	hr = _doc->selectSingleNode(L"/IMAGE/DESCRIPTION", &_description);
	if (hr == S_FALSE) {
		VARIANT val;
		val.vt = VT_I4;
		val.intVal = NODE_ELEMENT;
		hr = _doc->createNode(val, L"DESCRIPTION", NULL, &_description);
		test_hr("create node failed");
		IXMLDOMNode* out;
		hr = _doc->appendChild(_description, &out);
		test_hr("append child failed");
		if (out != NULL) {
			_description->Release();
			_description = out;
		}
	}
	test_hr("select node /IMAGE/DESCRIPTION failed");
	hr = _description->hasChildNodes(&res);
	if (!res) {
		IXMLDOMText* text;
		IXMLDOMNode* insert_text;
		_doc->createTextNode(L" ", &text);
		_description->appendChild(text, &insert_text);
		text->Release();
		insert_text->Release();
	}

	// Date
	hr = _doc->selectSingleNode(L"/IMAGE/DATE", &_date);
	if (hr == S_FALSE) {
		VARIANT val;
		val.vt = VT_I4;
		val.intVal = NODE_ELEMENT;
		hr = _doc->createNode(val, L"DATE", NULL, &_date);
		test_hr("create node failed");
		IXMLDOMNode* out;
		hr = _doc->appendChild(_date, &out);
		test_hr("append child failed");
		if (out != NULL) {
			_date->Release();
			_date = out;
		}
	}
	test_hr("select node /IMAGE/DATE failed");
	hr = _date->hasChildNodes(&res);
	if (!res) {
		IXMLDOMText* text;
		IXMLDOMNode* insert_text;
		_doc->createTextNode(L" ", &text);
		_date->appendChild(text, &insert_text);
		text->Release();
		insert_text->Release();
	}

	IXMLDOMNode* text;
	VARIANT val;

	_name->get_firstChild(&text);
	text->get_nodeValue(&val);
	oss << "name: " << val.bstrVal << std::endl;
	CoTaskMemFree(val.bstrVal);
	text->Release();

	_description->get_firstChild(&text);
	text->get_nodeValue(&val);
	oss << "description: " << val.bstrVal << std::endl;
	CoTaskMemFree(val.bstrVal);
	text->Release();

	_date->get_firstChild(&text);
	text->get_nodeValue(&val);
	oss << "date: " << val.bstrVal << std::endl;
	CoTaskMemFree(val.bstrVal);
	text->Release();


	Alert(oss.str());
#undef test_hr
}

WIM_ImageInfo::~WIM_ImageInfo()
{
	if (_doc != NULL) {
		_doc->Release();
		_name->Release();
		_description->Release();
		_date->Release();
	}
}

std::wstring WIM_ImageInfo::to_xml()
{
	return std::wstring();
}
