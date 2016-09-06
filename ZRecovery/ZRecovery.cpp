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
	auto path = dynamic_cast<TextBox*>(sender)->getText();
	WIM wim;
	if (WIM::test_file_exist(path)) {
		wim.open(path);
	}
	else {
		return;
	}
	wim.set_temporary_path(L"c:");

	auto lsv = dynamic_cast<ListView*>(html_ui_set.query("restore-wim-info").get());
	lsv->clear();
	for (size_t i = 0; i < wim.size(); ++i) {
		auto info = wim.get_info(i);
		lsv->insert({ std::to_wstring(i), info.get_name(), info.get_date_localtime(), info.get_description() });
	}
}

event(restore_on_browse_wim) {
	auto textbox_path = html_ui_set.query("restore-wim-path");
	auto path = WIM::open_wim_file();
	textbox_path->setText(path);
}
event(restore_on_select_backup) {
	auto lsv = dynamic_cast<ListView*>(sender);
	auto des = html_ui_set.query("restore-image-description");
	des->setText(lsv->at(EventArgs).colomns.back());
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
