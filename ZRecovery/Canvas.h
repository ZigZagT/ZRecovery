#pragma once
#include "WindowBase.h"

class Canvas : public WindowBase
{
public:
	Canvas(
		HWND parent,
		std::wstring className,
		RECT position = RECT{ 50, 50, 500, 300 }
	) :
		WindowBase(position, L"", WS_CHILD | WS_VISIBLE, parent, NULL)
	{
		_class_name = className;
	}
	Canvas(Canvas&& old) noexcept = default;
	virtual ~Canvas() {}


protected:
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void updateClass(WNDCLASSEXW& wc);
};