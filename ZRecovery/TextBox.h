#pragma once
#include "ControlBase.h"
#include "HTMLUI.h"

class TextBox : public ControlBase, public HTMLUI<TextBox>
{
public:
	TextBox(
		HWND parent,
		RECT position = RECT{ 0, 0, 200, 20 },
		std::wstring text = L"",
		DWORD style = 0) :
		ControlBase(position, text, WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_LEFT | WS_BORDER | style, parent, NULL, NULL) 
	{
		_class_name = L"EDIT";
	}

	TextBox(
		HWND parent,
		std::wstring text = L"",
		RECT position = RECT{ 0, 0, 200, 20 },
		DWORD style = 0) :
		ControlBase(position, text, WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_LEFT | WS_BORDER | style, parent, NULL, NULL)
	{
		_class_name = L"EDIT";
	}
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg)
		{
		case WM_COMMAND:
		{
			auto notify = HIWORD(wParam);
			switch (notify)
			{
			case EN_CHANGE:
			{
				if (onTextChange) {
					onTextChange(this, 0);
					return 0;
				}
				else {
					return -1;
				}
			}
			default:
				return DefWindowProc(_hwnd, uMsg, wParam, lParam);
			}
		}
		default:
			return DefWindowProc(_hwnd, uMsg, wParam, lParam);
		}
	}

	virtual ~TextBox() {}
	
	std::wstring getText() {
		static const size_t len = 256;
		static wchar_t buf[len];
		int res = GetWindowTextW(_hwnd, buf, len);
		return std::wstring(buf, res);
	}
	void setText(std::wstring text) {
		SetWindowText(_hwnd, text.c_str());
	}

	EventHandler onTextChange;

	// Inherited via HTMLUI
public:
	virtual void bind_event_handler(std::string event_name, IUIElement::EventHandler handler) override;
	static HTMLUI_TypeInfo::UIConstructor create_from_html;
	static HTMLUI_TypeInfo::UIMatchAttrMap match_attributes;
	static HTMLUI_TypeInfo::UISupportedEventsSet supported_events;
};

