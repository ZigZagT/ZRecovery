#include "stdafx.h"
#include "TextBox.h"

void TextBox::bind_event_handler(std::string event_name, IUIElement::EventHandler handler)
{
}
HTMLUI_TypeInfo::UIConstructor TextBox::create_from_html = [](HTMLUI_UIDescriptor& des) -> std::shared_ptr<UIBase> {
	auto ret = std::shared_ptr<UIBase>(new TextBox(des.parent, des.nameW(), des.position()), [](auto& p) {delete (TextBox*)p; });
	ret->create();
	return ret;
};
HTMLUI_TypeInfo::UIMatchAttrMap TextBox::match_attributes {
};
HTMLUI_TypeInfo::UISupportedEventsSet TextBox::supported_events {
};