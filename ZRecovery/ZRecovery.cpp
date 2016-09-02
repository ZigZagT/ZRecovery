// ZRecovery.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ZRecovery.h"
#include "UI.h"
#include "WIM.h"
#include "debug.h"
#include "load_resource.h"
#include "UIDemo.h"

// Global Variables:
HINSTANCE hInst;                                // ui instance

void ZRecovery();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
	hInst = hInstance;


	auto handlers = ui_demo();


	//ZRecovery();

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

void ZRecovery() {
	static std::list<std::shared_ptr<void*>> resources;
#pragma region macros_defines
#define create_resource(type, name, ...) \
	resources.emplace_back(new type(__VA_ARGS__), [](auto p) { \
		delete (type*)p; \
	}); \
	auto& name = *(type*)resources.back().get();
#pragma endregion
	auto wnd = SolidWindow(L"afafafaf");
	wnd.create();
	auto btn = Button(wnd.getHandler());
	btn.create();
	wnd.show(SW_SHOW);
	Sleep(3000);
}