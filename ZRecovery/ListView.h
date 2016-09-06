#pragma once
#include "ControlBase.h"
#include "HTMLUI.h"

class ListViewItem {
public:
	template <typename... Types>
	ListViewItem(std::wstring title, Types... args) :
		name(title),
		colomns{ args... }
	{}

	std::wstring name;
	std::vector<std::wstring> colomns;
};

class ListView : public ControlBase, public IUIContainer<ListViewItem>, public HTMLUI<ListView>
{
public:
	enum style_t {
		icon,
		small_icon,
		list,
		details,
		tile
	};
	ListView(
		HWND parent,
		std::wstring text = L"Button",
		RECT position = RECT{ 0, 0, 200, 100 },
		DWORD style = 0) : //LVS_SINGLESEL
		ControlBase(position, text, WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS | style, parent, NULL, NULL) {
		_window_class = WC_LISTVIEW;
	}
	virtual ~ListView() {
		destroy();
	}

	void setStyle(style_t s);
	void insert(ListViewItem& item);
	template <typename... Types>
	void setColumns(std::wstring&& col, Types&&... others) {
		clearColumns();
		_insertColumns<Types...>(std::forward<std::wstring&&>(col), std::forward<Types>(others)...);
	}
	template <typename ContainerT>
	void setColumns(ContainerT cols) {
		for (auto& str : cols) {
			_insertColumns(str);
		}
	}
	template <typename... Types>
	void insertColumn(std::wstring&& col, Types&&... others) {
		_insertColumns<Types...>(std::forward<std::wstring&&>(col), std::forward<Types>(others)...);
	}
	void setColumnWidth(size_t index, size_t width) {
		SendMessage(_hwnd, LVM_SETCOLUMNWIDTH, index, width);
	}
	void setColumnWidthAuto(size_t index) {
		SendMessage(_hwnd, LVM_SETCOLUMNWIDTH, index, LVSCW_AUTOSIZE);
	}
	void setColumnWidthAuto() {
		for (size_t i = 0; i < _count; ++i) {
			setColumnWidthAuto(i);
		}
	}
	size_t columnCount() {
		return _col_count;
	}

	virtual void refresh();
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg)
		{
		case WM_NOTIFY:
		{
			auto code = reinterpret_cast<NMHDR*>(lParam)->code;
			switch (code)
			{
			case LVN_GETDISPINFO:
			{
				auto& item = reinterpret_cast<NMLVDISPINFO*>(lParam)->item;
				if (item.iSubItem != 0) {
					if (_items[item.iItem].colomns.size() >= item.iSubItem) {
						item.pszText = (wchar_t*)_items[item.iItem].colomns[item.iSubItem - 1].c_str();
					}
					return TRUE;
				}
				return FALSE;
			}
			case NM_CLICK:
			{
				auto& info = *reinterpret_cast<NMITEMACTIVATE*>(lParam);
				if (onClick) {
					onClick(this, info.iItem);
				}
				return TRUE;
			}
			case LVN_ITEMACTIVATE:
			{
				auto& info = *reinterpret_cast<NMITEMACTIVATE*>(lParam);
				return 0;
			}
			case LVN_ITEMCHANGED: 
			{
				auto& info = *reinterpret_cast<NMLISTVIEW*>(lParam);
				if (onSelected) {
					if (info.uNewState | LVIS_SELECTED && !(info.uOldState &  LVIS_SELECTED)) {
						onSelected(this, info.iItem);
					}
				}
				return 0;
			}
			//case LVN_ODSTATECHANGED:
			//{
			//	auto& info = *reinterpret_cast<LPNMLVODSTATECHANGE*>(lParam);
			//	if (onSelectionChange) {
			//		onSelectionChange(this, (unsigned long long)&info);
			//	}
			//	return 0;
			//}
			default:
				return DefWindowProc(_hwnd, uMsg, wParam, lParam);
			}
		}
		default:
			return DefWindowProc(_hwnd, uMsg, wParam, lParam);
		}
	}

	EventHandler onSelectionChange;
	EventHandler onSelected;
	EventHandler onClick;

	// Inherited via IUIElement
	virtual void create() {
		ControlBase::create();
		SendMessage(_hwnd, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	}

	// Inherited via IUIContainer
	virtual void insert(ListViewItem && item) override;
	virtual ListViewItem & at(size_t index) override;
	virtual ListViewItem & operator[](size_t index) override;
	virtual size_t count() override;
	virtual void erase(size_t index) override;
	virtual ListViewItem & back() override;
	virtual ListViewItem & front() override;
	virtual void clear() override;

private:
	size_t _count = 0;
	size_t _col_count = 0;
	std::vector<ListViewItem> _items;

	void clearColumns() {
		for (; _col_count > 0; --_col_count) {
			ListView_DeleteColumn(_hwnd, _col_count - 1);
		}
	}
	template<typename... Types>
	void _insertColumns(std::wstring&& col, Types&&... others) {
		LVCOLUMN lv_col;
		lv_col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT | LVCF_SUBITEM;
		lv_col.fmt = LVCFMT_LEFT;
		lv_col.cx = 100;
		lv_col.iSubItem = static_cast<int>(_col_count);
		lv_col.pszText = (wchar_t*)col.c_str();
		ListView_InsertColumn(_hwnd, _col_count, &lv_col);
		++_col_count;

		_insertColumns(std::forward<Types>(others)...);
	}
	void _insertColumns() {
		return;
	}

	// Inherited via HTMLUI
public:
	virtual void bind_event_handler(std::string event_name, IUIElement::EventHandler handler) override;
	static HTMLUI_TypeInfo::UIConstructor create_from_html;
	static HTMLUI_TypeInfo::UIMatchAttrMap match_attributes;
	static HTMLUI_TypeInfo::UISupportedEventsSet supported_events;
};