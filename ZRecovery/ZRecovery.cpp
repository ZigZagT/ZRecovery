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

	ZRecovery();

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
	static std::vector<std::shared_ptr<void>> resources;
#pragma region macros_defines
#define create_resource(type, name, ...) \
	resources.emplace_back(reinterpret_cast<void*>(new type(__VA_ARGS__)), [](auto p) { \
		delete (type*)p; \
	}); \
	auto& name = *(type*)resources.back().get();
#pragma endregion
	create_resource(SolidWindow, wnd, load_resource<std::wstring>(hInst, IDC_ZRECOVERY), load_resource<std::wstring>(hInst, IDS_APP_TITLE), RECT{ 0, 0, 800, 600 });
	wnd.onClose = [](auto, auto) {
		PostQuitMessage(0);
	};
	wnd.create();
	create_resource(HTMLUI_UINode, html_ui);

	HTMLUI_Parser::named_ui_set html_ui_set;
	HTMLUI_TypeInfo::register_event_handler({
		{"checked", [](auto, auto) {
			Alert("checked!");
		}},
		{"cleared", [](auto, auto) {
			Alert("cleared!");
		}}
	});
	html_ui = HTMLUI_Parser::parse_file("ZRecovery.html");
	html_ui_set = HTMLUI_Parser::recursive_create(html_ui, wnd.getHandler());
	wnd.show(SW_SHOW);
}