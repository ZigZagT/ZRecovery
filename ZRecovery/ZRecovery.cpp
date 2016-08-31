// ZRecovery.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ZRecovery.h"
#include "UI.h"
#include "WIM.h"
#include "debug.h"
#include "load_resource.h"

// Global Variables:
HINSTANCE hInst;                                // ui instance

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	hInst = hInstance;

	auto wnd = SolidWindow(load_resource<std::wstring>(hInstance, IDC_ZRECOVERY), load_resource<std::wstring>(hInstance, IDS_APP_TITLE), RECT{0, 0, 800, 600});
	wnd.create();
	
	Tab tab(wnd.getHandler());
	tab.create();

	tab.insert(L"button");
	Button btn(tab[0].getHandler(), L"Button", RECT{ 50, 50, 150, 80 });
	btn.onClick = [](auto, auto) {
		Alert("Button clicked!");
	};
	btn.create();

	tab.insert(L"textbox");
	TextBox tbx(tab[1].getHandler(), RECT{ 50, 50, 250, 70 }, L"TextBox");
	tbx.create();
	Button btn_tbx(tab[1].getHandler(), L"Test", RECT{ 50, 90, 150, 120 });
	btn_tbx.onClick = [&tbx](auto s, auto) {
		//auto tbx = dynamic_cast<TextBox*>(s);
		auto txt = tbx.getText();
		Alert(std::wstring(L"TextBox content is: [") + txt  + L"]\n Add mark...");
		tbx.setText(std::wstring(L"Mark: [") + txt + L"]");
	};
	btn_tbx.create();

	tab.insert(L"label");
	Label lab(tab[2].getHandler(), L"Label", RECT{ 50, 50, 150, 80 });
	lab.create();
	//lab.setFont(Font(30).Create());

	tab.insert(L"listview");
	ListView lsv(tab[3].getHandler(), L"", RECT{ 50, 50, 700, 350 }, WS_BORDER);
	lsv.create();
	lsv.insert(ListViewItem{ L"haha1", L"t1", L"y1" });
	lsv.insert(ListViewItem{ L"haha2", L"t2", L"y2" });
	lsv.insert(ListViewItem{ L"haha3", L"t3", L"y3" });
	lsv.setStyle(lsv.details);
	lsv.setColumns(L"Col1", L"Col2", L"Col3");

	tab.insert(L"html ui");
	auto html = std::ifstream(R"abcd(index.html)abcd");
	std::ostringstream iss;
	iss << html.rdbuf();
	std::string html_str = iss.str();
	HTMLUI_TypeInfo::register_event_handler("html_button_click", [](auto btn, auto) {
		Alert(std::wstring(L"Html Button Clicked: ") + btn->getName());
	});
	auto& node = HTMLUI_Parser::Parse(html_str);
	HTMLUI_Parser::recursive_create(node, tab.back().getHandler());
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
