#include "stdafx.h"
#include "Tab.h"

void Tab::insert(std::wstring name)
{
	TabItem item;
	item.header.mask = TCIF_TEXT;
	item.header.pszText = const_cast<wchar_t*>(name.c_str());
	item.lParam = 0;
	if (TabCtrl_InsertItem(_hwnd, _index, &item) == -1) {
		throw std::runtime_error("tab insert item error");
	}
	_tab_content.emplace_back(_hwnd, _name + L"TAB::" + std::to_wstring(_index) + L"::" + name);
	auto& child = _tab_content.back();
	child.create();
	RECT rc;
	GetClientRect(_hwnd, &rc);
	TabCtrl_AdjustRect(_hwnd, FALSE, &rc);
	child.setPosition(rc);
	
	int i = TabCtrl_GetCurSel(_hwnd);
	if (_index == i) {
		ShowWindow(child.getHandler(), SW_SHOWNA);
	}
	else {
		ShowWindow(child.getHandler(), SW_HIDE);
	}

	++_index;
}

ContentWindow* Tab::at(size_t index) {
	return &_tab_content[index];
}
ContentWindow& Tab::operator[](size_t index) {
	return _tab_content[index];
}

void Tab::create()
{
	baseCreate();
	SendMessage(_hwnd, TCM_SETITEMEXTRA, (WPARAM)sizeof(void*), 0);
}

LRESULT Tab::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) 
	{
		case WM_NOTIFY:
		{
			NMHDR& nmhdr = *(NMHDR*)lParam;
			std::map<UINT, std::string> code;
			code[NM_CLICK] = "NM_CLICK";
			code[TCN_FOCUSCHANGE] = "TCN_FOCUSCHANGE";
			code[TCN_GETOBJECT] = "TCN_GETOBJECT";
			code[TCN_KEYDOWN] = "TCN_KEYDOWN";
			code[TCN_SELCHANGE] = "TCN_SELCHANGE";
			code[TCN_SELCHANGING] = "TCN_SELCHANGING";
			if (code.find(nmhdr.code) != code.end()) {
				//Alert(code[nmhdr.code]);
			}
			else {
				//Alert(wnd.getName() + L" : " + std::to_wstring(nmhdr.code));
			}
			if (nmhdr.hwndFrom == _hwnd) {
				switch (nmhdr.code)
				{
				case TCN_SELCHANGING:
				{
					_tab_content[TabCtrl_GetCurSel(_hwnd)].show(SW_HIDE);
					return FALSE;
				}
				case TCN_SELCHANGE:
				{
					_tab_content[TabCtrl_GetCurSel(_hwnd)].show(SW_SHOW);
				}
				default:
					break;
				}
			}
		}
		default:
		{
			return DefWindowProc(_hwnd, uMsg, wParam, lParam);
		}
	}
}