#pragma once
#include "stdafx.h"
#include "UIBase.h"

class DialogBase : public IUIElement
{
public:
	DialogBase(
		HWND parent,
		LPCTSTR dialog_template
	) {
		_parent = parent;
		_template = dialog_template;
	}
	virtual ~DialogBase() {
		unregister_window();
	}


protected:
	HWND _parent;
	LPCTSTR _template;

	HWND _hwnd;
	void baseCreate() {
		register_window();
		RECT rc;
		GetClientRect(_parent, &rc);
		setPosition(rc);
	}
	static INT_PTR  CALLBACK _dialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		DialogBase* pThis = nullptr;
		if (uMsg = WM_INITDIALOG) {
			return FALSE;
			pThis = (DialogBase*)lParam;
			return pThis->handleMessage(uMsg, wParam, lParam);
		}
		pThis = dynamic_cast<DialogBase*>(query_window_s(hwnd));
		return pThis->handleMessage(uMsg, wParam, lParam);
	}

	// Override IUIElement
public:
	virtual void create() = 0;
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

	virtual HWND getHandler() {
		return _hwnd;
	}
	virtual void register_window() {
		_dialog_registry.insert(std::make_pair(_hwnd, this));
	}
	virtual void unregister_window() {
		_dialog_registry.erase(_hwnd);
	}
	virtual IUIElement* query_window(HWND hwnd) {
		return query_window_s(hwnd);
	}
	static IUIElement* query_window_s(HWND hwnd) {
		return _dialog_registry.at(hwnd);
	}

	
	virtual void setPosition(RECT position) {
		MoveWindow(_hwnd, position.left, position.top, position.right - position.left, position.bottom - position.top, TRUE);
	}
	// not implemented
	virtual void setText(std::wstring text) {}
	virtual void setFont(HFONT) {}
	virtual void refresh() {}

private:
	static std::map<HWND, DialogBase*> _dialog_registry;
};

