// ZRecovery.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ZRecovery.h"
#include "UI.h"
#include "WIM.h"
#include "debug.h"
#include "load_resource.h"

// Global Variables:
HINSTANCE hInst;                                // current instance

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	hInst = hInstance;

	auto wnd = SolidWindow(load_resource<std::wstring>(hInstance, IDC_ZRECOVERY), load_resource<std::wstring>(hInstance, IDS_APP_TITLE));
	wnd.create();

	Tab tab(wnd.getHandler());
	tab.create();
	tab.insert(L"tab1");
	tab.insert(L"tab2");

	Button btn(tab.getHandler(), L"Button1", RECT{ 50, 50, 150, 80 });
	btn.onClick = [](HWND, LPARAM) {
		Alert("Button1 clicked!");
	};
	btn.create();

	Button btn2(tab.getHandler(), L"Button2", RECT{ 160, 50, 260, 80 });
	btn2.onClick = [](auto, auto) {
		Alert("Button2 clicked!");
	};
	btn2.create();

	Label lab(tab.getHandler(), L"Label", RECT{ 270, 50, 370, 80 });
	lab.create();
	lab.setFont(Font(30).Create());

	/*Button btn3(lab.getHandler(), L"Button3", 0, 30, 100, 30);
	btn3.onClick = [](auto, auto) {
		Alert("Button3 clicked!");
	};
	btn3.create();*/



	wnd.show(nCmdShow);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ZRECOVERY));
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int) msg.wParam;
}
