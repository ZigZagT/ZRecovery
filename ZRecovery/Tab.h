#pragma once
#include "stdafx.h"
#include "ControlBase.h"

class Tab : public ControlBase
{
public:
	Tab(HWND parent, RECT position) :
		ControlBase(position, L"", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, parent, NULL, NULL)
	{
		_window_class = WC_TABCONTROL;
	}
	Tab(HWND parent) :
		ControlBase(RECT{ 0,0,0,0 }, L"", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, parent, NULL, NULL)
	{
		_window_class = WC_TABCONTROL;
		RECT rc;
		GetClientRect(_parent, &rc);
		_position = rc;
	}
		
	void insert(std::wstring name);
	//void insert(std::wstring name, size_t position);

	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		Alert("Tab receive message " + std::to_string(uMsg));
		return 0;
	}
private:
	size_t _index = 0;
};

