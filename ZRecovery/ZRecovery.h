#pragma once
#include "UI.h"
#include "WIM.h"
#include "debug.h"
#include "load_resource.h"
#include "resource.h"

class ZRecovery {
public:
	ZRecovery() {
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
			{"backup_on_wim_path_change", backup_on_wim_path_change},
			{"backup_on_browse_wim", backup_on_browse_wim},
			{"backup_on_create_backup", backup_on_create_backup},

			{"restore_on_wim_path_change", restore_on_wim_path_change},
			{"restore_on_browse_wim", restore_on_browse_wim},
			{"restore_on_select_backup", restore_on_select_backup},
			{"restore_on_restore_to_selected_backup", restore_on_restore_to_selected_backup},
			{"restore_on_delete_selected_backup", restore_on_delete_selected_backup},

			{"factory_on_select_backup", factory_on_select_backup},
			{"factory_on_create_backup", factory_on_create_backup},
			{"factory_on_restore_to_selected_backup", factory_on_restore_to_selected_backup},
			{"factory_on_delete_selected_backup", factory_on_delete_selected_backup}
		});
		html_ui = HTMLUI_Parser::parse_file("ZRecovery.html");
		html_ui_set = HTMLUI_Parser::recursive_create(html_ui, wnd.getHandler());
		wnd.show(SW_SHOW);
	}
private:
	static std::vector<std::shared_ptr<void>> resources;
	static HTMLUI_Parser::named_ui_set html_ui_set;

#define event(name) static void name(IUIElement* sender, unsigned long long EventArgs)
	event(backup_on_wim_path_change);
	event(backup_on_browse_wim);
	event(backup_on_create_backup);

	event(restore_on_wim_path_change);
	event(restore_on_browse_wim);
	event(restore_on_select_backup);
	event(restore_on_restore_to_selected_backup);
	event(restore_on_delete_selected_backup);

	event(factory_on_select_backup);
	event(factory_on_create_backup);
	event(factory_on_restore_to_selected_backup);
	event(factory_on_delete_selected_backup);
#undef event
};