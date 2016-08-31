#include "stdafx.h"
#include "UIBase.h"

HFONT UIBase::_default_font = (HFONT)GetStockObject(DEFAULT_GUI_FONT); // CreateFont(18, 0, 0, 0, 400, FALSE, FALSE, FALSE, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"MS Shell Dlg");
std::map<HWND, UIBase*> UIBase::_window_registry;
