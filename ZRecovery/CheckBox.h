#pragma once
#include "stdafx.h"
#include "ControlBase.h"
#include "HTMLUI.h"

class CheckBox : public ControlBase, public HTMLUI<CheckBox>
{
public:
	CheckBox(
		HWND parent,
		std::wstring text = L"CheckBox",
		RECT position = RECT{ 0, 0, 100, 30 },
		DWORD style = 0
	) :
		ControlBase(position, text, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | BS_LEFTTEXT | style, parent, NULL, NULL)
	{
		_class_name = L"BUTTON";
	}
	CheckBox(CheckBox&& old) noexcept :
		ControlBase(std::move(old)),
		onToggle(std::move(old.onToggle)),
		onChecked(std::move(old.onChecked)),
		onClear(std::move(old.onClear))
	{}
	virtual ~CheckBox() {
		destroy();
	}

	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg)
		{
		case WM_COMMAND:
		{
			UINT cmd = LOWORD(wParam);
			switch (cmd)
			{
			case BN_CLICKED:
			{
				auto c = SendMessage(_hwnd, BM_GETCHECK, 0, 0);
				if (c == BST_CHECKED && onChecked) {
					onChecked(this, lParam);
				}
				if (c == BST_UNCHECKED && onClear) {
					onClear(this, lParam);
				}
				if (onToggle) {
					onToggle(this, lParam);
				}
				return 0;
			}
			default:
				return -1;
			}
		}
		default:
			return DefWindowProc(_hwnd, uMsg, wParam, lParam);
		}
	}

	EventHandler onChecked;
	EventHandler onClear;
	EventHandler onToggle;

	// Inherited via HTMLUI
public:
	virtual void bind_event_handler(std::string event_name, IUIElement::EventHandler handler) override;
	static HTMLUI_TypeInfo::UIConstructor create_from_html;
	static HTMLUI_TypeInfo::UIMatchAttrMap match_attributes;
	static HTMLUI_TypeInfo::UISupportedEventsSet supported_events;
};