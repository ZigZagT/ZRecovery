#pragma once
#include "stdafx.h"
#include "ControlBase.h"
#include "ContentWindow.h"

class Tab : public ControlBase
{
	typedef struct {
		TCITEMHEADER header;
		void*  lParam;
	} TabItem;
public:
	Tab(HWND parent, RECT position, DWORD style = 0) :
		ControlBase(position, L"", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | style, parent, NULL, NULL)
	{
		_window_class = WC_TABCONTROL;
	}
	Tab(HWND parent, DWORD style = 0) :
		ControlBase(RECT{ 0,0,0,0 }, L"", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | style, parent, NULL, NULL)
	{
		_window_class = WC_TABCONTROL;
		RECT rc;
		GetClientRect(_parent, &rc);
		_position = rc;
	}
		
	void insert(std::wstring name);
	ContentWindow* at(size_t index);
	ContentWindow& operator[](size_t index);
	//void insert(std::wstring name, size_t position);

	
private:
	size_t _index = 0;
	std::vector<ContentWindow> _tab_content;

	// Override IUIElement
public:
	virtual void create();
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

