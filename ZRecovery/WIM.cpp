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

WIM_ImageInfo WIM::get_info(size_t index)
{
	if (!_is_valid) {
		throw std::runtime_error("invalid handle");
	}
	HANDLE h = WIMLoadImage(_handle, index);
	if (h == NULL) {
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
	Alert(xml);
	return WIM_ImageInfo(xml);
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
	Alert(file.get());
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

