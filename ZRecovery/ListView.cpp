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

void ListView::refresh() {
	ListView_DeleteAllItems(_hwnd);
	_count = 0;

	LVITEM lv_item;
	lv_item.mask = LVIF_TEXT;
	lv_item.iSubItem = 0;
	for (; _count < _items.size(); ++_count) {
		lv_item.iItem = static_cast<int>(_count);
		lv_item.pszText = const_cast<wchar_t*>(_items[_count].name.c_str());
		ListView_InsertItem(_hwnd, &lv_item);
	}
	UpdateWindow(_hwnd);
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

void ListView::bind_event_handler(std::string event_name, IUIElement::EventHandler handler)
{
	if (case_insensitive_compare(event_name, "onSelectionChange")) {
		onSelectionChange = handler;
	}
	if (case_insensitive_compare(event_name, "onSelected")) {
		onSelected = handler;
	}
	if (case_insensitive_compare(event_name, "onChange")) {
		onSelected = handler;
	}
	if (case_insensitive_compare(event_name, "onClick")) {
		onClick = handler;
	}
}


HTMLUI_TypeInfo::UIConstructor ListView::create_from_html = [](HTMLUI_UIDescriptor& des) -> std::shared_ptr<UIBase> {
	enum state_marker : unsigned{
		table	=	0b0000000001,
		row		=	0b0000000010,
		//head	=	0b0000000100,
		//data	=	0b0000001000

	};
	static unsigned flag = 0;
	static std::shared_ptr<UIBase> ui;
	auto& lv = *dynamic_cast<ListView*>(ui.get());
	if (
		//std::find(des.classes.begin(), des.classes.end(), "LISTITEM") != des.classes.end()
		//| std::find(des.classes.begin(), des.classes.end(), "LISTVIEWITEM") != des.classes.end()
		(des.tag && des.tag.val() == "TABLE")
		) {
		flag = table;
		ui = std::shared_ptr<UIBase>(new ListView(des.parent, des.nameW(), des.position(), LVS_SINGLESEL), [](auto& p) {delete (ListView*)p; });
		ui->create();
		return ui;
	}
	if (flag & table && des.tag && des.tag.val() == "TH") {
		lv.insertColumn(des.nameW());
		lv.refresh();
		return ui;
	}
	if (flag & table && des.tag && des.tag.val() == "TD") {
		if (flag & row) {
			flag &= ~row;
			lv.insert({ des.nameW() });
		}
		else {
			lv.back().colomns.push_back(des.nameW());
		}
		lv.refresh();
		return ui;
	}
	if (flag & table && des.tag && des.tag.val() == "TR") {
		flag |= row;
		return ui;
	}
	return std::shared_ptr<UIBase>(nullptr);
};
HTMLUI_TypeInfo::UIMatchAttrMap ListView::match_attributes{
	//{ "class", "ListView" },
	//{ "class", "List" },
	//{ "class", "ListItem" },
	//{ "class", "ListViewItem" },
	{ "tag", "table"},
	{ "tag", "tr" },
	{ "tag", "th" },
	{ "tag", "td" }
};
HTMLUI_TypeInfo::UISupportedEventsSet ListView::supported_events{
	"onSelectionChange",
	"onSelected",
	"onChange",
	"onClick"
};