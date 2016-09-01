#pragma once
#include "stdafx.h"
#include "ControlBase.h"
#include "Debug.h"
#include "HTMLUI.h"

class Label : public ControlBase, public HTMLUI<Label> {
public:
	Label(
		HWND parent,
		std::wstring text = L"Label",
		RECT position = RECT{ 0, 0, 100, 30 },
		DWORD style = 0
	) :
		ControlBase(position, text, WS_CHILD | WS_VISIBLE | SS_CENTER | SS_NOTIFY | style, parent, NULL, NULL) {
		_class_name = L"STATIC";
	}

private:
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg)
		{
		case WM_NOTIFY:
		default:
			return DefWindowProc(_hwnd, uMsg, wParam, lParam);
		}
	}

	// Inherited via HTMLUI
public:
	virtual void bind_event_handler(std::string event_name, IUIElement::EventHandler handler) override;
	static HTMLUI_TypeInfo::UIConstructor create_from_html;
	static HTMLUI_TypeInfo::UIMatchAttrMap match_attributes;
	static HTMLUI_TypeInfo::UISupportedEventsSet supported_events;
};