#pragma once
#include "stdafx.h"
#include "ControlBase.h"

class IWindow {
public:
	virtual void show(int cmd) = 0;
	virtual void updateClass(WNDCLASSEXW& wc) = 0;
	virtual LRESULT dispatch_message(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
};

class WindowBase : public UIBase, virtual public IWindow
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
	~WindowBase() {}

	void setTitle(std::wstring title) { setText(title); }

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

		baseCreate();
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
		case WM_COMMAND:
			HWND hControl;
			hControl = reinterpret_cast<HWND>(lParam);
			try {
				auto& ptr = query_window(hControl);
				return ptr.handleMessage(uMsg, wParam, lParam);
			}
			catch (std::out_of_range) {
				goto handleMsg;
			}
		default:
			goto handleMsg;
		}
	handleMsg:
		return handleMessage(uMsg, wParam, lParam);
	}
	virtual void updateClass(WNDCLASSEXW& wc) = 0;
};

