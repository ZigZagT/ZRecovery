#pragma once
#include "stdafx.h"
#include "ControlBase.h"

class Button : public ControlBase
{
public:
	Button(
		HWND parent,
		std::wstring text = L"Button",
		RECT position = RECT{ 0, 0, 100, 30 }
	) :
		ControlBase(position, text, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, parent, NULL, NULL)
	{
		_class_name = L"BUTTON";
	}

	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
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