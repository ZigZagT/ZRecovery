#include "stdafx.h"
#include "UIDemo.h"

// use 	auto ui = ui_daemon();
std::vector<std::shared_ptr<UIBase>> ui_demo() {
#define create_ui(type, name, ...) \
	ret.emplace_back(new type(__VA_ARGS__), [](auto p) { \
		delete (type*)p; \
	}); \
	auto& name = *(type*)ret.back().get();


	auto hInstance = GetModuleHandle(NULL);
	std::vector<std::shared_ptr<UIBase>> ret;

	create_ui(SolidWindow, wnd, load_resource<std::wstring>(hInstance, IDC_ZRECOVERY), load_resource<std::wstring>(hInstance, IDS_APP_TITLE), RECT{ 0, 0, 800, 600 });
	wnd.create();

	create_ui(Tab, tab, wnd.getHandler());
	tab.create();

	tab.insert(L"button");
	create_ui(Button, btn, tab[0].getHandler(), L"Button", RECT{ 50, 50, 150, 80 });
	btn.onClick = [](auto, auto) {
		Alert("Button clicked!");
	};
	btn.create();

	tab.insert(L"textbox");
	create_ui(TextBox, tbx, tab[1].getHandler(), RECT{ 50, 50, 250, 70 }, L"TextBox");
	tbx.create();

	create_ui(Button, btn_tbx, tab[1].getHandler(), L"Test", RECT{ 50, 90, 150, 120 });
	btn_tbx.onClick = [&tbx](auto s, auto) {
		//auto tbx = dynamic_cast<TextBox*>(s);
		auto txt = tbx.getText();
		Alert(std::wstring(L"TextBox content is: [") + txt + L"]\n Add mark...");
		tbx.setText(std::wstring(L"Mark: [") + txt + L"]");
	};
	btn_tbx.create();

	tab.insert(L"label");
	create_ui(Label, lab, tab[2].getHandler(), L"Label", RECT{ 50, 50, 150, 80 });
	lab.create();
	//lab.setFont(Font(30).Create());

	tab.insert(L"listview");
	create_ui(ListView, lsv, tab[3].getHandler(), L"", RECT{ 50, 50, 700, 350 }, WS_BORDER);
	lsv.create();
	lsv.insert(ListViewItem{ L"haha1", L"t1", L"y1" });
	lsv.insert(ListViewItem{ L"haha2", L"t2", L"y2" });
	lsv.insert(ListViewItem{ L"haha3", L"t3", L"y3" });
	lsv.setStyle(lsv.details);
	lsv.setColumns(L"Col1", L"Col2", L"Col3");

	tab.insert(L"html ui");
	auto html = std::ifstream(R"abcd(index.html)abcd");
	std::ostringstream oss;
	oss << html.rdbuf();
	std::string html_str = oss.str();
	HTMLUI_TypeInfo::register_event_handler("html_button_click", [](auto btn, auto) {
		Alert(std::wstring(L"Html Button Clicked: ") + btn->getName());
	});
	HTMLUI_UINode* node = new HTMLUI_UINode;
	ret.emplace_back(reinterpret_cast<UIBase*>(node), [](auto p) {
		delete (HTMLUI_UINode*)p;
	});
	*node = HTMLUI_Parser::parse(html_str);
	HTMLUI_Parser::recursive_create(*node, tab.back().getHandler());
	wnd.show(SW_SHOW);

	return ret;

#undef create_ui
}