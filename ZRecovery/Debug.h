#pragma once

#include "stdafx.h"

inline void Alert(std::wstring msg) {
	MessageBoxW(NULL, msg.c_str(), L"Debug Message", 0);
}

inline void Alert(std::string msg) {
	MessageBoxA(NULL, msg.c_str(), "Debug Message", 0);
}