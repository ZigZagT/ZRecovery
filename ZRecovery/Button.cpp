#include "stdafx.h"
#include "Button.h"

HTMLUI_TypeInfo::UIConstructor Button::create_from_html = [](HTMLUI_UIDescriptor& des) -> std::shared_ptr<UIBase> {
	return std::shared_ptr<UIBase>(new Button(0), [](auto& p){delete (Button*)p; });
};

void Button::bind_event_handler(std::string event_name, IUIElement::EventHandler handler)
{
	if (HTMLUI_TypeInfo::case_insensitive_compare(event_name, "onClick")) {
		onClick = handler;
	}
}

HTMLUI_TypeInfo::UIMatchAttrMap Button::match_attributes {
	{"class", "button"}
};

HTMLUI_TypeInfo::UISupportedEventsSet Button::supported_events{
	"onClick"
};