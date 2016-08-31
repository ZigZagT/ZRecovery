#include "stdafx.h"
#include "HTMLUI.h"

std::vector<HTMLUI_TypeInfo> HTMLUI_TypeInfo::ui_typeinfo_set;
std::map<std::string, IUIElement::EventHandler> HTMLUI_TypeInfo::_event_handler_registry;