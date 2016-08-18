#pragma once
#include "stdafx.h"
#include "UIBase.h"
#include "ControlBase.h"

template <typename T>
class WindowBase : public UIBase
{
	friend class ControlBase;
public:
	WindowBase(
		std::wstring title = L"Window",
		int x = 50,
		int y = 50,
		int width = 500,
		int height = 300,
		DWORD style = WS_OVERLAPPEDWINDOW,
		HWND parent = NULL,
		HINSTANCE instance = NULL
	)
		:
		UIBase(x, y, width, height, title, style, parent, instance, this)
	{}
	~WindowBase() {}

	void setTitle(std::wstring title) { setName(title); }
	std::wstring getTitle() { return getName(); }

	void create() {
		_class_name = this->className();

		WNDCLASSEXW wc = { 0 };
		wc.cbSize = sizeof(WNDCLASSEXW);
		wc.lpfnWndProc = T::_windowProc;
		wc.hInstance = _instance;
		wc.lpszClassName = _class_name.c_str();
		updateClass(wc);
		RegisterClassExW(&wc);

		baseCreate();
	}
	void show(int cmd) {
		ShowWindow(_hwnd, cmd);
		UpdateWindow(_hwnd);
	}

protected:
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
	virtual std::wstring className() = 0;
	virtual void updateClass(WNDCLASSEXW& wc) = 0;
	LRESULT baseHandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_PAINT:
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(_hwnd, &ps);
			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
			EndPaint(_hwnd, &ps);
			return 0;
		case WM_COMMAND:
			HWND hControl;
			hControl = reinterpret_cast<HWND>(lParam);
			ControlBase::controlPtr ptr;
			ptr = ControlBase::queryControl(hControl);
			return ptr->handleCommand(uMsg, wParam, lParam);
		default:
			return DefWindowProc(_hwnd, uMsg, wParam, lParam);
		}
		return TRUE;
	}

private:
	static LRESULT CALLBACK _windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		T* pThis = nullptr;
		if (uMsg == WM_NCCREATE) {
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			pThis = reinterpret_cast<T*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));

			pThis->_hwnd = hwnd;
		}
		else {
			pThis = reinterpret_cast<T*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		}

		if (pThis) {
			return dynamic_cast<WindowBase*>(pThis)->handleMessage(uMsg, wParam, lParam);
		}
		else {
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
};

