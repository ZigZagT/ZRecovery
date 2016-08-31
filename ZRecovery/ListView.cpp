#include "stdafx.h"
#include "ListView.h"

void ListView::setStyle(style_t s) {
	if (_is_valid) {
		unsigned int view;
		switch (s)
		{
		case ListView::icon:
			view = LV_VIEW_ICON;
			break;
		case ListView::small_icon:
			view = LV_VIEW_SMALLICON;
			break;
		case ListView::list:
			view = LV_VIEW_LIST;
			break;
		case ListView::details:
			view = LV_VIEW_DETAILS;
			break;
		case ListView::tile:
			view = LV_VIEW_TILE;
			break;
		default:
			throw std::runtime_error("invalid style valie");
			break;
		}
		SendMessage(_hwnd, LVM_SETVIEW, view, 0);
	}
	else {
		//auto mask = LVS_ICON | LVS_SMALLICON | LVS_LIST | LVS_REPORT;
		auto old_val = _style & LVS_TYPEMASK;
		_style &= ~LVS_TYPEMASK;
		switch (s)
		{
		case ListView::icon:
			_style |= LVS_ICON;
			break;
		case ListView::small_icon:
			_style |= LVS_SMALLICON;
			break;
		case ListView::list:
			_style |= LVS_LIST;
			break;
		case ListView::details:
			_style |= LVS_REPORT;
			break;
		case ListView::tile:
			_style |= old_val;
			throw std::runtime_error("cannot set tile style before create");
			break;
		default:
			_style |= old_val;
			throw std::runtime_error("invalid style valie");
			break;
		}
	}
}

void ListView::insert(ListViewItem& item) {
	LVITEM lv_item;
	lv_item.mask = LVIF_TEXT;
	lv_item.iItem = static_cast<int>(_count);
	lv_item.iSubItem = 0;
	lv_item.pszText = const_cast<wchar_t*>(item.name.c_str());

	ListView_InsertItem(_hwnd, &lv_item);
	_items.push_back(item);
	++_count;
}

void ListView::insert(ListViewItem && item)
{
	insert(item);
}

ListViewItem & ListView::at(size_t index)
{
	return _items.at(index);
}

ListViewItem & ListView::operator[](size_t index)
{
	return _items[index];
}

size_t ListView::count()
{
	return _count;
}

void ListView::erase(size_t index)
{
	throw std::runtime_error("not implemented");
}

ListViewItem & ListView::back()
{
	return _items.back();
}

ListViewItem & ListView::front()
{
	return _items.front();
}
