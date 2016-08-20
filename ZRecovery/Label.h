#pragma once
#include "stdafx.h"
#include "ControlBase.h"
#include "Debug.h"

class Label : public ControlBase {
public:
	Label(
		HWND parent,
		std::wstring text = L"Label",
		RECT position = RECT{ 0, 0, 100, 30 }
	) :
		ControlBase(position, text, WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER | SS_NOTIFY, parent, NULL, NULL) {
		_class_name = L"STATIC";
	}

private:
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		auto cmd = LOWORD(wParam);
		Alert("message " + std::to_string(cmd));
		return 0;
	}
};