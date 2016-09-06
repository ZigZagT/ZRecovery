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

ZRecovery::ZRecovery()
{
	{
#pragma region macros_defines
#define create_resource(type, name, ...) \
	resources.emplace_back(reinterpret_cast<void*>(new type(__VA_ARGS__)), [](auto p) { \
		delete (type*)p; \
	}); \
	auto& name = *(type*)resources.back().get();
#pragma endregion
		extern HINSTANCE hInst;
		create_resource(SolidWindow, wnd, load_resource<std::wstring>(hInst, IDC_ZRECOVERY), load_resource<std::wstring>(hInst, IDS_APP_TITLE), RECT{ 0, 0, 800, 600 });
		wnd.onClose = [](auto, auto) {
			PostQuitMessage(0);
		};
		wnd.create();
		create_resource(HTMLUI_UINode, html_ui);

		HTMLUI_TypeInfo::register_event_handler({
			{ "backup_on_wim_path_change", backup_on_wim_path_change },
			{ "backup_on_browse_wim", backup_on_browse_wim },
			{ "backup_on_create_backup", backup_on_create_backup },

			{ "restore_on_wim_path_change", restore_on_wim_path_change },
			{ "restore_on_browse_wim", restore_on_browse_wim },
			{ "restore_on_select_backup", restore_on_select_backup },
			{ "restore_on_restore_to_selected_backup", restore_on_restore_to_selected_backup },
			{ "restore_on_delete_selected_backup", restore_on_delete_selected_backup },

			{ "factory_on_select_backup", factory_on_select_backup },
			{ "factory_on_create_backup", factory_on_create_backup },
			{ "factory_on_restore_to_selected_backup", factory_on_restore_to_selected_backup },
			{ "factory_on_delete_selected_backup", factory_on_delete_selected_backup }
		});
		html_ui = HTMLUI_Parser::parse_file("ZRecovery.html");
		html_ui_set = HTMLUI_Parser::recursive_create(html_ui, wnd.getHandler());

		init();
		wnd.show(SW_SHOW);
	}
}

void ZRecovery::init()
{
}

#define event(name) void ZRecovery::name(IUIElement* sender, unsigned long long EventArgs)

#pragma region page 1: backup
event(backup_on_wim_path_change) {
	auto checkbox_compress = html_ui_set.query("backup-compress");
	auto label_create_or_append = html_ui_set.query("backup-wim-create-or-append");

	auto path = dynamic_cast<TextBox*>(sender)->getText();
	if (WIM::test_file_exist(path)) {
		checkbox_compress->disable();
		label_create_or_append->setText(load_resource<std::wstring>(hInst, IDS_BACKUP_APPEND));
	}
	else {
		checkbox_compress->disable();
		label_create_or_append->setText(load_resource<std::wstring>(hInst, IDS_BACKUP_CREATE));
	}
}
event(backup_on_browse_wim) {
	auto textbox_path = html_ui_set.query("backup-wim-path");
	auto path = WIM::save_wim_file();
	textbox_path->setText(path);
}
event(backup_on_create_backup) {
	Alert("backup_on_create_backup");
}
#pragma endregion

#pragma region page 2: restore
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
#pragma endregion

#pragma region page 3: factory
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
#pragma endregion

#undef event
