#pragma once
#include "stdafx.h"
#include "ControlBase.h"
#include "Debug.h"

class Label : public ControlBase {
public:
	Label(
		HWND parent,
		std::wstring text,
		int x,
		int y,
		int width,
		int height		
	) :
		ControlBase(parent, text, x, y, width, height, WS_CHILD | WS_VISIBLE | SS_CENTER | SS_NOTIFY) {
		_class_name = L"STATIC";
	}

	void create() {
		baseCreate();
		registerControl();
		//Alert("created!");
		//EnableWindow(_hwnd, IsWindowEnabled(_hwnd));
		//UpdateWindow(_hwnd);
	}
private:
	virtual LRESULT handleCommand(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		auto cmd = LOWORD(wParam);
		Alert("message " + std::to_string(cmd));
		return 0;
	}
};