#pragma once
#include "WindowBase.h"

class SolidWindow : public WindowBase<SolidWindow>
{
public:
	SolidWindow(
		std::wstring className,
		std::wstring title = L"Solid Window",
		int x = 50,
		int y = 50,
		int width = 500,
		int height = 300,
		DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		HWND parent = NULL,
		HINSTANCE instance = NULL) :
		WindowBase<SolidWindow>(title, x, y, width, height, style, parent, instance)
	{
		_class_name = className;
	}
	~SolidWindow();


protected:
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual std::wstring className();
	virtual void updateClass(WNDCLASSEXW& wc);
};