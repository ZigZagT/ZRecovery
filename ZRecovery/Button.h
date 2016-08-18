#pragma once
#include "stdafx.h"
#include "ControlBase.h"

class Button : public ControlBase
{
public:
	Button(
		HWND parent,
		std::wstring text = L"Button",
		int x = 50,
		int y = 50,
		int width = 100,
		int height = 30,
		DWORD style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		HINSTANCE instance = NULL,
		LPVOID param = NULL
	) :
		ControlBase(parent, text, x, y, width, height, style, instance),
		onClick(nullptr)
	{
		_class_name = L"BUTTON";
	}

	virtual ~Button() {}

	void create() {
		baseCreate();
		registerControl();
	}
	virtual LRESULT handleCommand(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		UINT cmd;
		cmd = LOWORD(wParam);
		switch (cmd)
		{
		case BN_CLICKED:
			if (onClick) {
				onClick(_hwnd, lParam);
			}
			return 0;
		default:
			return -1;
		}
	}

	std::function<void(HWND, LPARAM)> onClick;
};