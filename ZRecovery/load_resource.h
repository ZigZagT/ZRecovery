#pragma once

#include "stdafx.h"
#include "resource.h"

template <typename T>
T load_resource(HINSTANCE hInst, UINT uID);
template<typename T>
T load_resource(HINSTANCE hInst, std::wstring id);

template <>
std::wstring load_resource<std::wstring>(HINSTANCE hInst, UINT uID);

template<>
std::wstring load_resource<std::wstring>(HINSTANCE hInst, std::wstring id);