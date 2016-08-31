#pragma once
#include "WindowBase.h"
#include "HTMLUI.h"

class Canvas : public WindowBase, public HTMLUI<Canvas>
{
public:
	Canvas(
		HWND parent,
		std::wstring className,
		RECT position = RECT{ 50, 50, 500, 300 },
		DWORD style = 0
	) :
		WindowBase(position, L"", WS_CHILD | WS_VISIBLE | style, parent, NULL)
	{
		_class_name = className;
	}
	Canvas(Canvas&& old) noexcept = default;
	virtual ~Canvas() {}


protected:
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void updateClass(WNDCLASSEXW& wc);

public:
	// Inherited via HTMLUI
	virtual void bind_event_handler(std::string event_name, IUIElement::EventHandler handler) override;
	static HTMLUI_TypeInfo::UIConstructor create_from_html;
	static HTMLUI_TypeInfo::UIMatchAttrMap match_attributes;
	static HTMLUI_TypeInfo::UISupportedEventsSet supported_events;
};