#pragma once
#include "WindowBase.h"

class Container : public WindowBase
{
public:
	Container(
		HWND parent,
		std::wstring className,
		RECT position = RECT{ 50, 50, 500, 300 }
	) :
		WindowBase(position, L"", WS_CHILD | WS_VISIBLE, parent, NULL)
	{
		_class_name = className;
	}
	Container(Container&&) noexcept = default;
	virtual ~Container() {}


protected:
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void updateClass(WNDCLASSEXW& wc);
};