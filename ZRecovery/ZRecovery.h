#pragma once
#include "UI.h"
#include "WIM.h"
#include "debug.h"
#include "load_resource.h"
#include "resource.h"

class ZRecovery {
public:
	ZRecovery();
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

	static void init();
};