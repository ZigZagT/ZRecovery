#pragma once
#include "stdafx.h"
#include "Debug.h"

class UIBase;
class IUIElement;

class IUIElement {
	using windowPtr = UIBase*;
public:
	virtual void create() = 0;
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
	virtual HWND getHandler() = 0;
	virtual void register_window() = 0;
	virtual void unregister_window() = 0;
	virtual UIBase& query_window(HWND hwnd) = 0;

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

	virtual ~UIBase() {
		unregister_window();
		DestroyWindow(_hwnd);

		try {
			if (_parent != NULL) {
				auto& p = query_window(_parent);
				p._children.erase(_hwnd);
			}
		}
		catch (std::out_of_range) {}
	};
	

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
			throw std::runtime_error("baseCreate create window failed");
		}
		register_window();
		setFont(_default_font);

		try {
			if (_parent != NULL) {
				auto& p = query_window(_parent);
				p._children.insert(_hwnd);
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
	virtual UIBase& query_window(HWND hwnd) {
		return *_window_registry.at(hwnd);
	}

	virtual void setPosition(RECT position) {
		//SetWindowPos(_hwnd, NULL, position.left, position.top, position.right - position.left, position.bottom - position.top, 0);
		MoveWindow(_hwnd, position.left, position.top, position.right - position.left, position.bottom - position.top, TRUE);
	}
	virtual void setText(std::wstring text) {
		SendMessage(_hwnd, WM_SETTEXT, (WPARAM)NULL, (LPARAM)text.c_str());
	}
	void setFont(HFONT font) {
		SendMessage(_hwnd, WM_SETFONT, (WPARAM)font, (LPARAM)TRUE);
	}
	void refresh() {
		UpdateWindow(_hwnd);
	}
private:
	static std::map<HWND, UIBase*> _window_registry;
};

