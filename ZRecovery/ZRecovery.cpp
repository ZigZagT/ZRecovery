// ZRecovery.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ZRecovery.h"

// Global Variables:
HINSTANCE hInst;                                // ui instance

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	hInst = hInstance;

	ZRecovery z;

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ZRECOVERY));
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}




std::vector<std::shared_ptr<void>> ZRecovery::resources;
HTMLUI_Parser::named_ui_set ZRecovery::html_ui_set;

#define event(name) void ZRecovery::name(IUIElement* sender, unsigned long long EventArgs)

// page 1
event(backup_on_wim_path_change) {
	Alert("backup_on_wim_path_change");
}
event(backup_on_browse_wim) {
	Alert("backup_on_browse_wim");
}
event(backup_on_create_backup) {
	Alert("backup_on_create_backup");
}

// page 2
event(restore_on_wim_path_change) {
	Alert("restore_on_wim_path_change");
}
event(restore_on_browse_wim) {
	auto path = WIM::open_wim_file();
	WIM wim;
	if (WIM::test_file_exist(path)) {
		wim.open(path);
	}
	wim.set_temporary_path(L"c:");
	auto info = wim.get_info(0);
	info.set_date_now();
	Alert(info.get_date_localtime());
	Alert(info.to_xml());
	html_ui_set.query("backup-wim-path");
}
event(restore_on_select_backup) {
	Alert("restore_on_select_backup");
}
event(restore_on_restore_to_selected_backup) {
	Alert("restore_on_restore_to_selected_backup");
}
event(restore_on_delete_selected_backup) {
	Alert("restore_on_delete_selected_backup");
}

// page 3
event(factory_on_select_backup) {
	Alert("factory_on_select_backup");
}
event(factory_on_create_backup) {
	Alert("factory_on_create_backup");
}
event(factory_on_restore_to_selected_backup) {
	Alert("factory_on_restore_to_selected_backup");
}
event(factory_on_delete_selected_backup) {
	Alert("factory_on_delete_selected_backup");
}

#undef event
