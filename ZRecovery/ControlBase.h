#pragma once
#include "stdafx.h"
#include "UIBase.h"
#include "WindowBase.h"

class ControlBase : public UIBase
{
	template <typename>
	friend class WindowBase;
	using controlPtr = ControlBase*;
public:
	ControlBase(
		HWND parent = NULL,
		std::wstring text = L"Control",
		int x = 50,
		int y = 50,
		int width = 100,
		int height = 30,
		DWORD style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		HINSTANCE instance = NULL
	):
		UIBase(x, y, width, height, text, style, parent, instance, nullptr)
	{}
	virtual ~ControlBase() {}

	std::wstring getText() { return getName(); }
	void setText(std::wstring text) { setName(text); }

protected:
	void registerControl() {
		//auto ptr = std::make_shared<ControlBase>(this);
		_control_registry.insert(std::make_pair(_hwnd, this));
	}
	static controlPtr queryControl(HWND hwnd) {
		return _control_registry[hwnd];
	}
	virtual LRESULT handleCommand(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

private:
	static std::map<HWND, controlPtr> _control_registry;
};

