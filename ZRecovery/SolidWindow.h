#pragma once
#include "WindowBase.h"

class SolidWindow : public WindowBase
{
public:
	SolidWindow(
		std::wstring className,
		std::wstring title = L"Solid Window",
		RECT position = RECT{ 50, 50, 500, 300 }
	) :
		WindowBase(position, title, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, NULL, NULL)
	{
		_class_name = className;
	}
	SolidWindow(SolidWindow&&) noexcept = default;
	virtual ~SolidWindow() {
		destroy();
	}

protected:
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void updateClass(WNDCLASSEXW& wc);
};