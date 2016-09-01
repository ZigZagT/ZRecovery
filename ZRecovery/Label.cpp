#include "stdafx.h"
#include "Label.h"

void Label::bind_event_handler(std::string event_name, IUIElement::EventHandler handler)
{
}

HTMLUI_TypeInfo::UIConstructor Label::create_from_html = [](HTMLUI_UIDescriptor& des) -> std::shared_ptr<UIBase> {
	DWORD style = 0;
	if (des.border) {
		style |= WS_BORDER;
	}
	auto ret = std::shared_ptr<UIBase>(new Label(des.parent, des.nameW(), des.position(), style), [](auto& p) {delete (Label*)p; });
	ret->create();
	return ret;
};
HTMLUI_TypeInfo::UIMatchAttrMap Label::match_attributes{
	{"class", "label"},
	{"tag", "label"},
	{"tag", "p"}
};
HTMLUI_TypeInfo::UISupportedEventsSet Label::supported_events{

};