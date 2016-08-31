#include "stdafx.h"
#include "HTMLtoUI_Daemon.h"
#include "UI.h"

HTMLtoUI_Daemon HTMLtoUI_Daemon::_UI_Daemon;

HTMLtoUI_Daemon::HTMLtoUI_Daemon()
{
	Button::HTMLtoUI_Enable();
	Canvas::HTMLtoUI_Enable();
}


HTMLtoUI_Daemon::~HTMLtoUI_Daemon()
{
}
