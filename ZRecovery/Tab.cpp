#include "stdafx.h"
#include "Tab.h"

void Tab::insert(std::wstring name)
{
	TCITEM item;
	item.mask = TCIF_TEXT;
	item.pszText = const_cast<wchar_t*>(name.c_str());
	if (TabCtrl_InsertItem(_hwnd, _index, &item) == -1) {
		throw std::runtime_error("tab insert item error");
	}
	++_index;
}
