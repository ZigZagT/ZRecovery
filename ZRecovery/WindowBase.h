#pragma once
#include "stdafx.h"
#include "ControlBase.h"

class IWindow {
public:
	virtual void show(int cmd) = 0;
	virtual void updateClass(WNDCLASSEXW& wc) = 0;
	virtual LRESULT dispatch_message(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
};

class WindowBase : public UIBase, public IWindow
{
public:
	WindowBase(
		RECT position = RECT{ 50, 50, 500, 300 },
		std::wstring title = L"Window",
		DWORD style = WS_OVERLAPPEDWINDOW,
		HWND parent = NULL,
		HINSTANCE instance = NULL
	)
		:
		UIBase(position, title, style, parent, instance, this)
	{}
	WindowBase(WindowBase&) = delete;
	WindowBase(WindowBase&& old) noexcept :
	UIBase(std::move(old))
	{
		SetWindowLongPtr(_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
	}
	virtual ~WindowBase() {
		destroy();
	}

	WindowBase& operator= (WindowBase&& old) noexcept {
		UIBase::operator=(std::move(old));
		SetWindowLongPtr(_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		return *this;
	}

	void setTitle(std::wstring title) { setText(title); }

	EventHandler onClose;

protected:
	HBRUSH background = (HBRUSH)(COLOR_WINDOW + 1);

private:
	static LRESULT CALLBACK _windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		WindowBase* pThis = nullptr;
		if (uMsg == WM_NCCREATE) {
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			pThis = reinterpret_cast<WindowBase*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));

			pThis->_hwnd = hwnd;
		}
		else {
			pThis = reinterpret_cast<WindowBase*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			try {
				query_window_s(hwnd);
			}
			catch (std::out_of_range) {
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
			}
		}

		if (pThis) {
			return dynamic_cast<WindowBase*>(pThis)->dispatch_message(uMsg, wParam, lParam);
		}
		else {
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}

	// IUIElement
public:
	virtual void create() {
		WNDCLASSEXW wc = { 0 };
		wc.cbSize = sizeof(WNDCLASSEXW);
		wc.lpfnWndProc = _windowProc;
		wc.hInstance = _instance;
		wc.lpszClassName = _class_name.c_str();
		updateClass(wc);
		RegisterClassExW(&wc);

		UIBase::create();
	}
	virtual void destroy() {
		if (_is_valid) {
			UIBase::destroy();
			UnregisterClass(_class_name.c_str(), _instance);
		}
	}
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

	// IWindow
	virtual void show(int cmd) {
		ShowWindow(_hwnd, cmd);
		UpdateWindow(_hwnd);
	}
	LRESULT dispatch_message(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg)
		{
		case WM_CTLCOLORBTN:
			return (LRESULT)background;
		case WM_CTLCOLORSTATIC:
			return (LRESULT)background;
		case WM_NOTIFY:
		{
			NMHDR& nmhdr = *(NMHDR*)lParam;
			if ((UINT_PTR)wParam != nmhdr.idFrom) {
				Alert("WM_NOTIFY: wParam invalid!");
			}
			auto k = _children.size();
			if (_children.find(nmhdr.hwndFrom) == _children.end()) {
				goto defaultHandleMessage;
			}
			IUIElement* wnd;
			try {
				wnd = query_window(nmhdr.hwndFrom);
			}
			catch (std::out_of_range) {
				goto defaultHandleMessage;
			}
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
				//Alert(wnd->getName() + L" : " + std::to_wstring(nmhdr.code));
			}
			return wnd->handleMessage(uMsg, wParam, lParam);
			break;
		}
		case WM_COMMAND:
		{
			if (lParam == 0) {
				goto defaultHandleMessage;
			}
			HWND hControl = reinterpret_cast<HWND>(lParam);
			try {
				auto wnd = query_window(hControl);
				return wnd->handleMessage(uMsg, wParam, lParam);
				break;
			}
			catch (std::out_of_range) {
				//Alert("WM_COMMAND invalid hwnd");
				goto defaultHandleMessage;
			}
		}
		default:
			goto defaultHandleMessage;
		}
	defaultHandleMessage:
		return this->handleMessage(uMsg, wParam, lParam);
	}
	virtual void updateClass(WNDCLASSEXW& wc) = 0;
};

