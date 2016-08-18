#include "stdafx.h"
#include "COM_Daemon.h"

COM_Daemon COM_Daemon::_start_daemon;

COM_Daemon::COM_Daemon()
{
	auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE);
	if (FAILED(hr)) {
		throw std::runtime_error("CoInitializeEx failed");
	}
}


COM_Daemon::~COM_Daemon()
{
	CoUninitialize();
}
