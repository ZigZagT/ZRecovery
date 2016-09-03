#include "stdafx.h"
#include "CheckBox.h"

void CheckBox::bind_event_handler(std::string event_name, IUIElement::EventHandler handler)
{
	if (case_insensitive_compare(event_name, "onChecked")) {
		onChecked = handler;
	}
	if (case_insensitive_compare(event_name, "onClear")) {
		onClear = handler;
	}
	if (case_insensitive_compare(event_name, "onToggle")) {
		onToggle = handler;
	}
}

HTMLUI_TypeInfo::UIConstructor CheckBox::create_from_html = [](HTMLUI_UIDescriptor& des) -> std::shared_ptr<UIBase> {
	auto ret = std::shared_ptr<UIBase>(new CheckBox(des.parent, des.nameW(), des.position()), [](auto& p) {delete (CheckBox*)p; });
	ret->create();
	return ret;
};

HTMLUI_TypeInfo::UIMatchAttrMap CheckBox::match_attributes{
	//{ "type", "checkbox" },
	{ "class", "checkbox" }
};

HTMLUI_TypeInfo::UISupportedEventsSet CheckBox::supported_events{
	"onChecked",
	"onClear",
	"onToggle"
};