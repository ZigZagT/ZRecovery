#pragma once
#include "stdafx.h"
#include "UIBase.h"
#include "WindowBase.h"

class ControlBase : public UIBase
{
public:
	ControlBase() {}
	ControlBase(RECT position, HWND parent) {
		_position = position;
		_parent = parent;
	}
	ControlBase(
		RECT position,
		std::wstring name,
		DWORD style,
		HWND parent,
		HINSTANCE instance,
		LPVOID param
	) :
		UIBase(position, name, style, parent, instance, param)
	{}
	ControlBase(ControlBase&) = delete;

	ControlBase(ControlBase&& old) noexcept = default;

	virtual ~ControlBase() {
		destroy();
	}

};

