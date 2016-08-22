#pragma once
#include "stdafx.h"
#include "ControlBase.h"

class Button : public ControlBase
{
public:
	Button(
		HWND parent,
		std::wstring text = L"Button",
		RECT position = RECT{ 0, 0, 100, 30 },
		DWORD style = 0
	) :
		ControlBase(position, text, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | style, parent, NULL, NULL)
	{
		_class_name = L"BUTTON";
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
				if (onClick) {
					onClick(_hwnd, lParam);
				}
				return 0;
			default:
				return -1;
			}
		}
		default:
			return DefWindowProc(_hwnd, uMsg, wParam, lParam);
		}
	}

	std::function<void(HWND, LPARAM)> onClick;
};