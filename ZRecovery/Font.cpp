#include "stdafx.h"
#include "Font.h"


Font::Font()
{
}

Font::Font(int size, int weight, std::wstring face) :
	Height(size),
	Weight(weight),
	Fac(face)
{
}


Font::~Font()
{
}
