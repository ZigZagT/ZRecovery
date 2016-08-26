#pragma once
#include "stdafx.h"
#include "ControlBase.h"
#include "Container.h"

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
	Container* at(size_t index);
	Container& operator[](size_t index);
	//void insert(std::wstring name, size_t position);

	
private:
	size_t _index = 0;
	std::vector<Container> _tab_content;

	// Override IUIElement
public:
	virtual void create();
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

