#pragma once
#include "DialogBase.h"
class ModelessDialog : public DialogBase
{
public:
	using proxy_t = std::map<UINT, std::function<LRESULT(WPARAM, LPARAM)>>;
	ModelessDialog(
		HWND parent,
		LPCTSTR dialog_template,
		proxy_t proxy = {}) :
		DialogBase(parent, dialog_template) {
		this->proxy = proxy;
	}
	virtual ~ModelessDialog() {}

	virtual void create() {
		_hwnd = CreateDialogParam(NULL, _template, _parent, _dialogProc, reinterpret_cast<LPARAM>(this));
		baseCreate();
	}
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		auto& it = proxy.find(uMsg);
		if (it == proxy.end()) {
			return FALSE;
		}
		else {
			return it->second(wParam, lParam);
		}
	}
	proxy_t proxy;
};

