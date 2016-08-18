#include "stdafx.h"
#include "WIM.h"
#include "ZRecovery.h"
#include "load_resource.h"
#include "debug.h"


WIM::WIM()
{
}

WIM::~WIM()
{
}

WIM WIM::Open()
{	
	extern HINSTANCE hInst;
	IFileOpenDialog *pFileOpen;
	auto hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
	if (FAILED(hr)) {
		throw std::runtime_error("Create FileOpenDialog failed");
	}
	auto str = load_resource < std::wstring >(hInst, IDS_WIM_FILE);
	COMDLG_FILTERSPEC available_file_types[] = {
		{ str.c_str(), L"*.wim" },
		{ L"Any", L"*.*" }
	};
	pFileOpen->SetFileTypes(sizeof(available_file_types) / sizeof(COMDLG_FILTERSPEC), available_file_types);

	FILEOPENDIALOGOPTIONS opt;
	//pFileOpen->GetOptions(&opt);
	//pFileOpen->SetOptions(opt);
	hr = pFileOpen->Show(NULL);
	if (FAILED(hr))
	{
		throw std::runtime_error("FileOpenDialog.Show failed");
	}
	IShellItem* item;
	hr = pFileOpen->GetResult(&item);
	if (FAILED(hr))
	{
		throw std::runtime_error("GetResult failed");
	}
	LPWSTR file;
	item->GetDisplayName(SIGDN_FILESYSPATH, &file);
	Alert(file);
	CoTaskMemFree(file);
	return WIM();
}
