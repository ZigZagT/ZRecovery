#include "stdafx.h"
#include "TextBlock.h"

void TextBlock::bind_event_handler(std::string event_name, IUIElement::EventHandler handler)
{
	if (case_insensitive_compare(event_name, "onTextChange")) {
		onTextChange = handler;
	}
	else if (case_insensitive_compare(event_name, "onChange")) {
		onTextChange = handler;
	}
}
HTMLUI_TypeInfo::UIConstructor TextBlock::create_from_html = [](HTMLUI_UIDescriptor& des) -> std::shared_ptr<UIBase> {
	auto ret = std::shared_ptr<UIBase>(new TextBlock(des.parent, des.nameW(), des.position()), [](auto& p) {delete (TextBox*)p; });
	ret->create();
	return ret;
};
HTMLUI_TypeInfo::UIMatchAttrMap TextBlock::match_attributes{
	{ "class", "textblock" },
};
HTMLUI_TypeInfo::UISupportedEventsSet TextBlock::supported_events{
	"onTextChange",
	"onChange"
};