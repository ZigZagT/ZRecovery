#pragma once
#include "stdafx.h"

class UIBase
{
public:
	UIBase(
		int x = 50,
		int y = 50,
		int width = 100,
		int height = 30,
		std::wstring name = L"UI Element",
		DWORD style = WS_OVERLAPPED,
		HWND parent = NULL,
		HINSTANCE instance = NULL,
		LPVOID param = NULL) :
		_x(x),
		_y(y),
		_width(width),
		_height(height),
		_name(name),
		_style(style),
		_parent(parent),
		_instance(instance),
		_param(param),

		_hwnd(NULL),
		_class_name(L"INVALID_UI_CLASS")
		{}

	virtual ~UIBase() {};

	void setX(int x) { _x = x; }
	void setY(int y) { _y = y; }
	void setWidth(int width) { _width = width; }
	void setHeight(int height) { _height = height; }
	void setName(std::wstring name) { _name = name; }
	void setStyle(DWORD style) { _style = style; }
	void setParent(HWND parent) { _parent = parent; }
	void setInstance(HINSTANCE instance) { _instance = instance; }
	void setParam(LPVOID param) { _param = param; }

	int getX() { return _x; }
	int getY() { return _y; }
	int getWidth() { return _width; }
	int getHeight() { return _height; }
	std::wstring getName() { return _name; }
	DWORD getStyle() { return _style; }
	HWND getParent() { return _parent; }
	HINSTANCE getInstance() { return _instance; }
	LPVOID getParam() { return _param; }
	
	HWND getHandler() { return _hwnd; }
	void setFont(HFONT font) {
		SendMessage(_hwnd, WM_SETFONT, (WPARAM)font, (LPARAM)TRUE);
	}

protected:
	int _x;
	int _y;
	int _width;
	int _height;
	std::wstring _name;
	DWORD _style;
	HWND _parent;
	HINSTANCE _instance;
	LPVOID _param;

	HWND _hwnd;
	std::wstring _class_name;

	void baseCreate() {

		if (_parent != NULL && _instance == NULL) {
			_instance = (HINSTANCE)GetWindowLong(_parent, GWLP_HINSTANCE);
		}
		else if (_instance == NULL) {
			_instance = GetModuleHandle(NULL);
		}

		_hwnd = CreateWindow(
			_class_name.c_str(),
			_name.c_str(),
			_style,
			_x,
			_y,
			_width,
			_height,
			_parent,
			NULL,
			_instance,
			_param
		);
		//HFONT font;
		//font = CreateFont(18, 0, 0, 0, 400, FALSE, FALSE, FALSE, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
		//SendMessage(_hwnd, WM_SETFONT, (WPARAM)font, (LPARAM)TRUE);
		setFont(_default_font);
	}

private:
	static HFONT _default_font;
};

