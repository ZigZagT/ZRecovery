#pragma once
#include "WindowBase.h"

class ContentWindow : public WindowBase
{
public:
	ContentWindow(
		HWND parent,
		std::wstring className,
		RECT position = RECT{ 50, 50, 500, 300 }
	) :
		WindowBase(position, L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_OVERLAPPED, parent, NULL)
	{
		_class_name = className;
	}
	ContentWindow(ContentWindow&&) noexcept = default;
	virtual ~ContentWindow() {}


protected:
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void updateClass(WNDCLASSEXW& wc);
};