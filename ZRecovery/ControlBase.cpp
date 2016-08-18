#include "stdafx.h"
#include "ControlBase.h"

std::map<HWND, ControlBase::controlPtr> ControlBase::_control_registry;