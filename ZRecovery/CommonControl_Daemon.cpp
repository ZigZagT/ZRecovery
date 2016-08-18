#include "stdafx.h"
#include "CommonControl_Daemon.h"

CommonControl_Daemon CommonControl_Daemon::_common_control_daemon;

CommonControl_Daemon::CommonControl_Daemon()
{
	INITCOMMONCONTROLSEX control;
	control.dwSize = sizeof(control);

	std::vector<DWORD> ICCs = {
		ICC_STANDARD_CLASSES
	};

	for (auto icc : ICCs) {
		control.dwICC = icc;
		//InitCommonControls();
		InitCommonControlsEx(&control);
	}
}


CommonControl_Daemon::~CommonControl_Daemon()
{
}
