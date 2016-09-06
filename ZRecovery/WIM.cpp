#include "stdafx.h"
#include "WIM.h"
#include "ZRecovery.h"
#include "load_resource.h"
#include "debug.h"
#include "string_man.h"
#include "COM_Proxy.h"


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
		WIMUnregisterMessageCallback(_handle, reinterpret_cast<FARPROC>(&message_procedure));
	}
}

void WIM::open(std::wstring filepath)
{
	if (_is_valid) {
		WIMCloseHandle(_handle);
		WIMUnregisterMessageCallback(_handle, reinterpret_cast<FARPROC>(&message_procedure));
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

	auto res = WIMRegisterMessageCallback(_handle, reinterpret_cast<FARPROC>(&message_procedure), this);
	if (res == INVALID_CALLBACK_VALUE) {
		auto k = GetLastError();
		throw std::runtime_error("register message callback failed");
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
		auto k = GetLastError();
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
		auto k = GetLastError();
		throw std::runtime_error("get information failed");
	}

	std::wstring xml(static_cast<wchar_t*>(info), size / sizeof(wchar_t));
	//Alert(xml);
	return WIM_ImageInfo(xml);
}

void WIM::set_info(size_t index, WIM_ImageInfo info)
{
	if (!_is_valid) {
		throw std::runtime_error("invalid handle");
	}
	HANDLE h = WIMLoadImage(_handle, index + 1);
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

DWORD WIM::message_procedure(DWORD dwMessageId, WPARAM wParam, LPARAM lParam, PVOID pvUserData)
{
	WIM* wim = reinterpret_cast<WIM*>(pvUserData);

	std::vector<std::wstring> bypass = {
		LR"rule(\$windows.~bt)rule",
		LR"rule(\$windows.~ls)rule",
		LR"rule(\winpepge.sys)rule",
		LR"rule(\Windows\CSC)rule",
		LR"rule(\Recycled)rule",
		LR"rule(\Recycler)rule",
		LR"rule(\$Recycle.Bin)rule",
		LR"rule(\System Volume Information)rule",
		LR"rule(\swapfile.sys)rule",
		LR"rule(\pagefile.sys)rule",
		LR"rule(\hiberfil.sys)rule"
	};
	switch (dwMessageId)
	{
	case WIM_MSG_PROCESS:
	{
		std::wstring file = reinterpret_cast<wchar_t*>(wParam);
		BOOL* res = reinterpret_cast<BOOL*>(lParam);
		for (auto& rule : bypass) {
			auto loc = file.find(rule);
			if (loc != file.npos && loc < 3) {
				*res = 0;
				break;
			}
		}
		if (wim->onSkipFile) {
			wim->onSkipFile(wim, wParam, 0);
		}
		return WIM_MSG_SUCCESS;
	}
	case WIM_MSG_PROGRESS:
	{
		if (wim->onProgressChange) {
			wim->onProgressChange(wim, wParam, lParam);
		}
		return WIM_MSG_SUCCESS;
	}
	case WIM_MSG_ERROR:
	{
		if (wim->onError) {
			wim->onError(wim, wParam, lParam);
		}
		return WIM_MSG_SUCCESS;
	}
	default:
		return WIM_MSG_SUCCESS;
	}
}
















#define test_hr(msg) if (FAILED(hr)) { k = GetLastError();	throw std::runtime_error(msg);	}

WIM_ImageInfo::WIM_ImageInfo(std::wstring xml)
{
	if (xml[0] == leading_code) {
		xml[0] = ' ';
	}

	DWORD k;
	HRESULT hr;
	VARIANT_BOOL res;

	COM_Proxy<IXMLDOMNode> doc_node;

	hr = CoCreateInstance(CLSID_DOMDocument60, nullptr, CLSCTX_ALL, IID_IXMLDOMDocument2, reinterpret_cast<void**>(_doc.pget()));
	test_hr("create IXMLDocument failed");
	_doc.active();

	hr = _doc->loadXML(const_cast<BSTR>(xml.c_str()), &res);
	test_hr("loadXML failed");

	hr = _doc->get_firstChild(doc_node.pget());
	test_hr("get root element failed");
	doc_node.active();

	//std::wstringstream oss;
	//BSTR str;

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
	hr = _doc->selectSingleNode(L"/IMAGE/NAME", _name.pget());
	if (hr == S_FALSE) {
		CComVariant val = NODE_ELEMENT;
		hr = _doc->createNode(val, L"NAME", NULL, _name.pget());
		test_hr("create node failed");
		_name.active();

		COM_Proxy<IXMLDOMNode> out;
		hr = doc_node->appendChild(_name.get(), out.pget());
		test_hr("append child faied");
		if (out.get() != nullptr) {
			out.active();
			_name = out;
		}
	}
	test_hr("select node /IMAGE/NAME failed");
	_name.active();
	hr = _name->hasChildNodes(&res);
	test_hr("test hasChildNodes failed");
	if (!res) {
		COM_Proxy<IXMLDOMText> text;
		COM_Proxy<IXMLDOMNode> insert_text;

		hr = _doc->createTextNode(L" ", text);
		test_hr("create text node failed");
		text.active();

		hr = _name->appendChild(text, insert_text);
		test_hr("insert text failed");
		insert_text.active();
	}

	// Description
	hr = _doc->selectSingleNode(L"/IMAGE/DESCRIPTION", _description.pget());
	if (hr == S_FALSE) {
		CComVariant val = NODE_ELEMENT;
		hr = _doc->createNode(val, L"DESCRIPTION", NULL, _description.pget());
		test_hr("create node failed");
		_description.active();

		COM_Proxy<IXMLDOMNode> out;
		hr = doc_node->appendChild(_description.get(), out.pget());
		test_hr("append child faied");
		if (out.get() != nullptr) {
			out.active();
			_description = out;
		}
	}
	test_hr("select node /IMAGE/DESCRIPTION failed");
	_description.active();
	hr = _description->hasChildNodes(&res);
	test_hr("test hasChildNodes failed");
	if (!res) {
		COM_Proxy<IXMLDOMText> text;
		COM_Proxy<IXMLDOMNode> insert_text;

		hr = _doc->createTextNode(L" ", text);
		test_hr("create text node failed");
		text.active();

		hr = _description->appendChild(text, insert_text);
		test_hr("insert text failed");
		insert_text.active();
	}

	// Date
	hr = _doc->selectSingleNode(L"/IMAGE/DATE", _date.pget());
	if (hr == S_FALSE) {
		CComVariant val = NODE_ELEMENT;
		hr = _doc->createNode(val, L"DATE", NULL, _date.pget());
		test_hr("create node failed");
		_date.active();

		COM_Proxy<IXMLDOMNode> out;
		hr = doc_node->appendChild(_date.get(), out.pget());
		test_hr("append child faied");
		if (out.get() != nullptr) {
			out.active();
			_date = out;
		}
	}
	test_hr("select node /IMAGE/DATE failed");
	_date.active();
	hr = _date->hasChildNodes(&res);
	test_hr("test hasChildNodes failed");
	if (!res) {
		COM_Proxy<IXMLDOMText> text;
		COM_Proxy<IXMLDOMNode> insert_text;

		hr = _doc->createTextNode(L" ", text);
		test_hr("create text node failed");
		text.active();

		hr = _date->appendChild(text, insert_text);
		test_hr("insert text failed");
		insert_text.active();
	}


	//COM_Proxy<IXMLDOMNode> text;
	//CComVariant val;

	//text.reset();
	//_name->get_firstChild(text);
	//text.active();
	//text->get_nodeValue(&val);
	//oss << "name: " << val.bstrVal << std::endl;

	//text.reset();
	//_description->get_firstChild(text);
	//text.active();
	//text->get_nodeValue(&val);
	//oss << "description: " << val.bstrVal << std::endl;

	//text.reset();
	//_date->get_firstChild(text);
	//text.active();
	//text->get_nodeValue(&val);
	//oss << "date: " << val.bstrVal << std::endl;


	//Alert(oss.str());
}

WIM_ImageInfo::~WIM_ImageInfo()
{}

std::wstring WIM_ImageInfo::get_name()
{
	DWORD k;
	HRESULT hr;

	COM_Proxy<IXMLDOMNode> text;
	CComVariant val;

	hr = _name->get_firstChild(text);
	test_hr("get first child failed");
	text.active();
	hr = text->get_nodeValue(&val);
	test_hr("get node value failed");

	return std::wstring(val.bstrVal);
}

std::wstring WIM_ImageInfo::get_description()
{
	DWORD k;
	HRESULT hr;

	COM_Proxy<IXMLDOMNode> text;
	CComVariant val;

	hr = _description->get_firstChild(text);
	test_hr("get first child failed");
	text.active();
	hr = text->get_nodeValue(&val);
	test_hr("get node value failed");

	return std::wstring(val.bstrVal);
}

std::wstring WIM_ImageInfo::get_date()
{
	DWORD k;
	HRESULT hr;

	COM_Proxy<IXMLDOMNode> text;
	CComVariant val;

	hr = _date->get_firstChild(text);
	test_hr("get first child failed");
	text.active();
	hr = text->get_nodeValue(&val);
	test_hr("get node value failed");

	return std::wstring(val.bstrVal);
}

std::wstring WIM_ImageInfo::get_date_localtime()
{
	try {
		std::wostringstream oss;
		time_t time = std::stoll(get_date());
		oss << std::put_time(localtime(&time), L"%F %T %Z");
		return oss.str();
	}
	catch (std::invalid_argument) {
		return L"";
	}
}

void WIM_ImageInfo::set_name(std::wstring name)
{
	DWORD k;
	HRESULT hr;

	COM_Proxy<IXMLDOMNode> text;
	CComVariant val(name.c_str());

	hr = _name->get_firstChild(text);
	test_hr("get first child failed");
	text.active();
	hr = text->put_nodeValue(val);
	test_hr("set node value failed");
}

void WIM_ImageInfo::set_description(std::wstring description)
{
	DWORD k;
	HRESULT hr;

	COM_Proxy<IXMLDOMNode> text;
	CComVariant val(description.c_str());

	hr = _description->get_firstChild(text);
	test_hr("get first child failed");
	text.active();
	hr = text->put_nodeValue(val);
	test_hr("set node value failed");
}

void WIM_ImageInfo::set_date(std::wstring date)
{
	DWORD k;
	HRESULT hr;

	COM_Proxy<IXMLDOMNode> text;
	CComVariant val(date.c_str());

	hr = _date->get_firstChild(text);
	test_hr("get first child failed");
	text.active();
	hr = text->put_nodeValue(val);
	test_hr("set node value failed");
}

void WIM_ImageInfo::set_date_now()
{
	time_t t = time(NULL);
	set_date(std::to_wstring(t));
}

std::wstring WIM_ImageInfo::to_xml()
{
	HRESULT hr;
	BSTR str;
	DWORD k;

	hr = _doc->get_xml(&str);
	test_hr("get xml failed");

	std::wstring ret(str);
	if (ret[0] != leading_code) {
		ret.insert(0, 1, leading_code);
	}
	return ret;
}

#undef test_hr
