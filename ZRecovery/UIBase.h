#pragma once
#include "stdafx.h"
#include "Debug.h"

class UIBase;
class IUIElement;

class IUIElement {
public:
	virtual void create() = 0;
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
	virtual HWND getHandler() = 0;
	virtual void register_window() = 0;
	virtual void unregister_window() = 0;
	virtual IUIElement* query_window(HWND hwnd) = 0;

	virtual std::wstring getName() = 0;
	virtual void setPosition(RECT position) = 0;
	virtual void setText(std::wstring text) = 0;
	virtual void setFont(HFONT) = 0;
	virtual void refresh() = 0;
};

class UIBase : virtual public IUIElement
{
public:
	UIBase() {}
	UIBase(
		RECT position,
		std::wstring name,
		DWORD style,
		HWND parent,
		HINSTANCE instance,
		LPVOID param) :
		_position(position),
		_name(name),
		_style(style),
		_parent(parent),
		_instance(instance),
		_param(param)
	{}
	UIBase(UIBase&) = delete;
	UIBase(UIBase&& old) noexcept :
		_position(old._position),
		_name(old._name),
		_style(old._style),
		_parent(old._parent),
		_children(std::move(old._children)),
		_instance(old._instance),
		_param(old._param),
		_hwnd(old._hwnd),
		_class_name(old._class_name),
		_window_class(old._window_class),
		_is_valid(old._is_valid)
	{
		old._is_valid = false;
		if (_is_valid) {
			_window_registry[_hwnd] = this;
		}
	}

	virtual ~UIBase() {
		if (!_is_valid) {
			return;
		}
		unregister_window();
		DestroyWindow(_hwnd);

		try {
			if (_parent != NULL) {
				auto p = query_window(_parent);
				dynamic_cast<UIBase*>(p)->_children.erase(_hwnd);
			}
		}
		catch (std::out_of_range) {}
	};
	
	virtual std::wstring getName() { return _name; }

protected:
	RECT _position = RECT{ 0, 0, 500, 300 };
	std::wstring _name = L"UI Element";
	DWORD _style = WS_OVERLAPPED;
	HWND _parent = NULL;
	std::set<HWND> _children = {};
	HINSTANCE _instance = NULL;
	LPVOID _param = NULL;

	HWND _hwnd = NULL;
	std::wstring _class_name = L"INVALID_UI_CLASS";
	LPCTSTR _window_class = NULL;
	bool _is_valid = false;

	void baseCreate() {

		if (_parent != NULL && _instance == NULL) {
			_instance = (HINSTANCE)GetWindowLong(_parent, GWLP_HINSTANCE);
		}
		else if (_instance == NULL) {
			_instance = GetModuleHandle(NULL);
		}
		if (_window_class == NULL) {
			_window_class = _class_name.c_str();
		}
		_hwnd = CreateWindow(
			_window_class,
			_name.c_str(),
			_style,
			_position.left,
			_position.top,
			_position.right - _position.left,
			_position.bottom - _position.top,
			_parent,
			NULL,
			_instance,
			_param
		);

		if (_hwnd == NULL) {
			auto k = GetLastError();
			throw std::runtime_error("baseCreate create window failed");
		}

		_is_valid = true;
		register_window();
		setFont(_default_font);
		refresh();

		try {
			if (_parent != NULL) {
				auto p = query_window(_parent);
				dynamic_cast<UIBase*>(p)->_children.insert(_hwnd);
			}
		}
		catch (std::out_of_range) {}
	}

private:
	static HFONT _default_font;

// Implementation of IUIElement
public:
	virtual void create() {
		baseCreate();
	}
	//virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	//	return DefWindowProc(_hwnd, uMsg, wParam, lParam);
	//}
	virtual HWND getHandler() { return _hwnd; }
	virtual void register_window() {
		_window_registry.insert(std::make_pair(_hwnd, this));
	}
	virtual void unregister_window() {
		_window_registry.erase(_hwnd);
	}
	virtual IUIElement* query_window(HWND hwnd) {
		return query_window_s(hwnd);
	}
	static IUIElement* query_window_s(HWND hwnd) {
		return _window_registry.at(hwnd);
	}

	virtual void setPosition(RECT position) {
		//SetWindowPos(_hwnd, NULL, position.left, position.top, position.right - position.left, position.bottom - position.top, 0);
		MoveWindow(_hwnd, position.left, position.top, position.right - position.left, position.bottom - position.top, TRUE);
	}
	virtual void setText(std::wstring text) {
		SendMessage(_hwnd, WM_SETTEXT, (WPARAM)NULL, (LPARAM)text.c_str());
	}
	void setFont(HFONT font) {
		SendMessage(_hwnd, WM_SETFONT, (WPARAM)font, (LPARAM)FALSE);
	}
	void refresh() {
		UpdateWindow(_hwnd);
	}
private:
	static std::map<HWND, UIBase*> _window_registry;
};

