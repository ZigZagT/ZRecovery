#pragma once

#include "stdafx.h"
class Font
{
public:
	Font();
	Font(int size, int weight = 400, std::wstring face = L"MS Shell Dlg");
	~Font();
	HFONT Create() {
		return CreateFont(Height, Width, Escapement, Orientation, Weight, Italic, Underline, StrikeOut, CharSet, OutputPrecision, ClipPrecision, Quality, PitchAndFamily, Fac.c_str());
	}

	int     Height = 18;
	int     Width = 0;
	int     Escapement = 0;
	int     Orientation = 0;
	int     Weight = 400;
	DWORD   Italic = FALSE;
	DWORD   Underline = FALSE;
	DWORD   StrikeOut = FALSE;
	DWORD   CharSet = GB2312_CHARSET;
	DWORD   OutputPrecision = OUT_DEFAULT_PRECIS;
	DWORD   ClipPrecision = CLIP_DEFAULT_PRECIS;
	DWORD   Quality = CLEARTYPE_QUALITY;
	DWORD   PitchAndFamily = DEFAULT_PITCH;
	std::wstring Fac = L"MS Shell Dlg";
};

