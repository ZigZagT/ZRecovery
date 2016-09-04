#pragma once
#include "stdafx.h"
#include "gumbo.h"
#include "katana.h"
#include "UIBase.h"
#include "HTMLtoUI_Daemon.h"
#include "string_man.h"
#include "load_resource.h"

template <typename>
class HTMLUI;
class IHTMLUI;
class HTMLUI_UINode;
class HTMLUI_Parser;
class HTMLUI_UIDescriptor;
class HTMLUI_TypeInfo;

class HTMLUI_TypeInfo {
	template <typename UIType>
	friend class HTMLUI;
	friend class HTMLUI_Parser;
public:
	using UIConstructor = std::function<std::shared_ptr<UIBase>(HTMLUI_UIDescriptor&)>;
	using UIMatchAttrMap = std::multimap<std::string, std::string>;
	using UISupportedEventsSet = std::set<std::string>;

	UIMatchAttrMap& match_attributes;
	UISupportedEventsSet& supported_events;
	UIConstructor& create;

	static IUIElement::EventHandler get_event_handler(std::string handler_id) {
		auto it = _event_handler_registry.find(handler_id);
		if (it != _event_handler_registry.end()) {
			return it->second;
		}
		else {
			return IUIElement::EventHandler();
		}
	}
	static void register_event_handler(std::initializer_list<std::pair<std::string, IUIElement::EventHandler>> list) {
		for (auto& p : list) {
			register_event_handler(p.first, p.second);
		}
	}
	static void register_event_handler(std::string handler_id, IUIElement::EventHandler handler) {
		_event_handler_registry.insert(std::make_pair(handler_id, handler));
	}
private:
	static std::map<std::string, IUIElement::EventHandler> _event_handler_registry;
	static std::vector<HTMLUI_TypeInfo> ui_typeinfo_set;

	HTMLUI_TypeInfo(UIConstructor& constructor, std::multimap<std::string, std::string>& attributes, std::set<std::string>& events) :
		create(constructor),
		match_attributes(attributes),
		supported_events(events)
	{
		ui_typeinfo_set.push_back(*this);
	}
};

class HTMLUI_UIDescriptor {

public:
	template<typename T>
	class Nullable : public std::unique_ptr<T> {
	public:
		Nullable() : std::unique_ptr<T>(nullptr), is_null(true) {}
		template<typename... Types>
		Nullable(Types... args) : std::unique_ptr<T>(new T(std::forward<Types>(args)...)), is_null(false) {}

		Nullable& operator=(T& val) {
			this->is_null = false;
			if (std::is_default_constructible<T>::value && std::is_assignable<T&, T&>::value) {
				(*this) = val;
				return *this;
			}
			else {
				this->reset(new T(val));
				return *this;
			}
		}
		operator bool() {
			return !is_null;
		}
		//operator T&() {
		//	return *(this->get());
		//}
		T& val() {
			return *(this->get());
		}
		bool is_null = true;
	};
	HTMLUI_UIDescriptor() {}
	void update(GumboVector& attr_vector) {
#define add_attr_start() if(false) {}
#define add_attr_str(attr_name) else if (select(#attr_name)) { \
						attr_name = attr.value; \
	}
#define add_attr_str2(attr_name, var_name) else if (select(#attr_name)) { \
		var_name = attr.value; \
	}
#define add_attr_int(attr_name) else if (select(#attr_name)) { \
		attr_name = std::stoi(attr.value); \
	}
#define push_class_name() else if (select("class")) { \
						classes.push_back(attr.value); \
						for (auto& c : classes.back()) { \
							c = toupper(c); \
						} \
	}
#define add_attr_end() ;

		for (size_t i = 0; i < attr_vector.length; ++i) {
			auto attr = *(GumboAttribute*)attr_vector.data[i];
			auto select = std::bind(case_insensitive_compare, attr.name, std::placeholders::_1);
			add_attr_start()
				add_attr_int(x)
				add_attr_int(y)
				add_attr_int(width)
				add_attr_int(height)
				add_attr_int(left)
				add_attr_int(top)
				add_attr_int(right)
				add_attr_int(bottom)
				add_attr_int(border)

				add_attr_str(src)
				add_attr_str(name)
				add_attr_str2(text, name)
				add_attr_str2(id, sid)

				push_class_name()
			add_attr_end()
		}

#undef push_class_name
#undef add_attr_start
#undef add_attr_str
#undef add_attr_str2
#undef add_attr_int
#undef add_attr_end
	}
	void update(KatanaOutput& katana) {
#define add_attr_start() if(false) {}
#define add_attr_str(attr_name, value) else if (select(#attr_name)) { \
						attr_name = value; \
	}
#define add_attr_cast_to_int(attr_name, value) else if (select(#attr_name)) { \
						attr_name = static_cast<int>(value); \
	}
#define add_attr_end() ;
		if (katana.errors.length > 0) {
			return;
		}

		switch (katana.mode)
		{
		case KatanaParserModeDeclarationList:
		{
			for (size_t i = 0; i < katana.declarations->length; ++i) {
				auto dec = static_cast<KatanaDeclaration*>(katana.declarations->data[i]);
				auto select = std::bind(case_insensitive_compare, dec->property, std::placeholders::_1);
				add_attr_start()
					add_attr_cast_to_int(width, static_cast<KatanaValue*>(dec->values->data[0])->fValue)
					add_attr_cast_to_int(height, static_cast<KatanaValue*>(dec->values->data[0])->fValue)
					add_attr_cast_to_int(left, static_cast<KatanaValue*>(dec->values->data[0])->fValue)
					add_attr_cast_to_int(top, static_cast<KatanaValue*>(dec->values->data[0])->fValue)
					add_attr_cast_to_int(right, static_cast<KatanaValue*>(dec->values->data[0])->fValue)
					add_attr_cast_to_int(bottom, static_cast<KatanaValue*>(dec->values->data[0])->fValue)
					add_attr_cast_to_int(border, static_cast<KatanaValue*>(dec->values->data[0])->fValue)
				add_attr_end()
			}
			break;
		}
		case KatanaParserModeStylesheet:
		case KatanaParserModeRule:
		case KatanaParserModeKeyframeRule:
		case KatanaParserModeKeyframeKeyList:
		case KatanaParserModeMediaList:
		case KatanaParserModeValue:
		case KatanaParserModeSelector:
		default:
			// TODO: support more css style
			break;
		}
#undef add_attr_start
#undef add_attr_str
#undef add_attr_cast_to_int
#undef add_attr_end
	}
	void update(GumboTag tag) {
		switch (tag)
		{
		case GUMBO_TAG_HTML:
		{
			this->tag = "HTML";
			break;
		}
		case GUMBO_TAG_HEAD:
		{
			this->tag = "HEAD";
			break;
		}
		case GUMBO_TAG_TITLE:
		{
			this->tag = "TITLE";
			break;
		}
		case GUMBO_TAG_BASE:
		{
			this->tag = "BASE";
			break;
		}
		case GUMBO_TAG_LINK:
		{
			this->tag = "LINK";
			break;
		}
		case GUMBO_TAG_META:
		{
			this->tag = "META";
			break;
		}
		case GUMBO_TAG_STYLE:
		{
			this->tag = "STYLE";
			break;
		}
		case GUMBO_TAG_SCRIPT:
		{
			this->tag = "SCRIPT";
			break;
		}
		case GUMBO_TAG_NOSCRIPT:
		{
			this->tag = "NOSCRIPT";
			break;
		}
		case GUMBO_TAG_TEMPLATE:
		{
			this->tag = "TEMPLATE";
			break;
		}
		case GUMBO_TAG_BODY:
		{
			this->tag = "BODY";
			break;
		}
		case GUMBO_TAG_ARTICLE:
		{
			this->tag = "ARTICLE";
			break;
		}
		case GUMBO_TAG_SECTION:
		{
			this->tag = "SECTION";
			break;
		}
		case GUMBO_TAG_NAV:
		{
			this->tag = "NAV";
			break;
		}
		case GUMBO_TAG_ASIDE:
		{
			this->tag = "ASIDE";
			break;
		}
		case GUMBO_TAG_H1:
		{
			this->tag = "H1";
			break;
		}
		case GUMBO_TAG_H2:
		{
			this->tag = "H2";
			break;
		}
		case GUMBO_TAG_H3:
		{
			this->tag = "H3";
			break;
		}
		case GUMBO_TAG_H4:
		{
			this->tag = "H4";
			break;
		}
		case GUMBO_TAG_H5:
		{
			this->tag = "H5";
			break;
		}
		case GUMBO_TAG_H6:
		{
			this->tag = "H6";
			break;
		}
		case GUMBO_TAG_HGROUP:
		{
			this->tag = "HGROUP";
			break;
		}
		case GUMBO_TAG_HEADER:
		{
			this->tag = "HEADER";
			break;
		}
		case GUMBO_TAG_FOOTER:
		{
			this->tag = "FOOTER";
			break;
		}
		case GUMBO_TAG_ADDRESS:
		{
			this->tag = "ADDRESS";
			break;
		}
		case GUMBO_TAG_P:
		{
			this->tag = "P";
			break;
		}
		case GUMBO_TAG_HR:
		{
			this->tag = "HR";
			break;
		}
		case GUMBO_TAG_PRE:
		{
			this->tag = "PRE";
			break;
		}
		case GUMBO_TAG_BLOCKQUOTE:
		{
			this->tag = "BLOCKQUOTE";
			break;
		}
		case GUMBO_TAG_OL:
		{
			this->tag = "OL";
			break;
		}
		case GUMBO_TAG_UL:
		{
			this->tag = "UL";
			break;
		}
		case GUMBO_TAG_LI:
		{
			this->tag = "LI";
			break;
		}
		case GUMBO_TAG_DL:
		{
			this->tag = "DL";
			break;
		}
		case GUMBO_TAG_DT:
		{
			this->tag = "DT";
			break;
		}
		case GUMBO_TAG_DD:
		{
			this->tag = "DD";
			break;
		}
		case GUMBO_TAG_FIGURE:
		{
			this->tag = "FIGURE";
			break;
		}
		case GUMBO_TAG_FIGCAPTION:
		{
			this->tag = "FIGCAPTION";
			break;
		}
		case GUMBO_TAG_MAIN:
		{
			this->tag = "MAIN";
			break;
		}
		case GUMBO_TAG_DIV:
		{
			this->tag = "DIV";
			break;
		}
		case GUMBO_TAG_A:
		{
			this->tag = "A";
			break;
		}
		case GUMBO_TAG_EM:
		{
			this->tag = "EM";
			break;
		}
		case GUMBO_TAG_STRONG:
		{
			this->tag = "STRONG";
			break;
		}
		case GUMBO_TAG_SMALL:
		{
			this->tag = "SMALL";
			break;
		}
		case GUMBO_TAG_S:
		{
			this->tag = "S";
			break;
		}
		case GUMBO_TAG_CITE:
		{
			this->tag = "CITE";
			break;
		}
		case GUMBO_TAG_Q:
		{
			this->tag = "Q";
			break;
		}
		case GUMBO_TAG_DFN:
		{
			this->tag = "DFN";
			break;
		}
		case GUMBO_TAG_ABBR:
		{
			this->tag = "ABBR";
			break;
		}
		case GUMBO_TAG_DATA:
		{
			this->tag = "DATA";
			break;
		}
		case GUMBO_TAG_TIME:
		{
			this->tag = "TIME";
			break;
		}
		case GUMBO_TAG_CODE:
		{
			this->tag = "CODE";
			break;
		}
		case GUMBO_TAG_VAR:
		{
			this->tag = "VAR";
			break;
		}
		case GUMBO_TAG_SAMP:
		{
			this->tag = "SAMP";
			break;
		}
		case GUMBO_TAG_KBD:
		{
			this->tag = "KBD";
			break;
		}
		case GUMBO_TAG_SUB:
		{
			this->tag = "SUB";
			break;
		}
		case GUMBO_TAG_SUP:
		{
			this->tag = "SUP";
			break;
		}
		case GUMBO_TAG_I:
		{
			this->tag = "I";
			break;
		}
		case GUMBO_TAG_B:
		{
			this->tag = "B";
			break;
		}
		case GUMBO_TAG_U:
		{
			this->tag = "U";
			break;
		}
		case GUMBO_TAG_MARK:
		{
			this->tag = "MARK";
			break;
		}
		case GUMBO_TAG_RUBY:
		{
			this->tag = "RUBY";
			break;
		}
		case GUMBO_TAG_RT:
		{
			this->tag = "RT";
			break;
		}
		case GUMBO_TAG_RP:
		{
			this->tag = "RP";
			break;
		}
		case GUMBO_TAG_BDI:
		{
			this->tag = "BDI";
			break;
		}
		case GUMBO_TAG_BDO:
		{
			this->tag = "BDO";
			break;
		}
		case GUMBO_TAG_SPAN:
		{
			this->tag = "SPAN";
			break;
		}
		case GUMBO_TAG_BR:
		{
			this->tag = "BR";
			break;
		}
		case GUMBO_TAG_WBR:
		{
			this->tag = "WBR";
			break;
		}
		case GUMBO_TAG_INS:
		{
			this->tag = "INS";
			break;
		}
		case GUMBO_TAG_DEL:
		{
			this->tag = "DEL";
			break;
		}
		case GUMBO_TAG_IMAGE:
		{
			this->tag = "IMAGE";
			break;
		}
		case GUMBO_TAG_IMG:
		{
			this->tag = "IMG";
			break;
		}
		case GUMBO_TAG_IFRAME:
		{
			this->tag = "IFRAME";
			break;
		}
		case GUMBO_TAG_EMBED:
		{
			this->tag = "EMBED";
			break;
		}
		case GUMBO_TAG_OBJECT:
		{
			this->tag = "OBJECT";
			break;
		}
		case GUMBO_TAG_PARAM:
		{
			this->tag = "PARAM";
			break;
		}
		case GUMBO_TAG_VIDEO:
		{
			this->tag = "VIDEO";
			break;
		}
		case GUMBO_TAG_AUDIO:
		{
			this->tag = "AUDIO";
			break;
		}
		case GUMBO_TAG_SOURCE:
		{
			this->tag = "SOURCE";
			break;
		}
		case GUMBO_TAG_TRACK:
		{
			this->tag = "TRACK";
			break;
		}
		case GUMBO_TAG_CANVAS:
		{
			this->tag = "CANVAS";
			break;
		}
		case GUMBO_TAG_MAP:
		{
			this->tag = "MAP";
			break;
		}
		case GUMBO_TAG_AREA:
		{
			this->tag = "AREA";
			break;
		}
		case GUMBO_TAG_MATH:
		{
			this->tag = "MATH";
			break;
		}
		case GUMBO_TAG_MI:
		{
			this->tag = "MI";
			break;
		}
		case GUMBO_TAG_MO:
		{
			this->tag = "MO";
			break;
		}
		case GUMBO_TAG_MN:
		{
			this->tag = "MN";
			break;
		}
		case GUMBO_TAG_MS:
		{
			this->tag = "MS";
			break;
		}
		case GUMBO_TAG_MTEXT:
		{
			this->tag = "MTEXT";
			break;
		}
		case GUMBO_TAG_MGLYPH:
		{
			this->tag = "MGLYPH";
			break;
		}
		case GUMBO_TAG_MALIGNMARK:
		{
			this->tag = "MALIGNMARK";
			break;
		}
		case GUMBO_TAG_ANNOTATION_XML:
		{
			this->tag = "ANNOTATION_XML";
			break;
		}
		case GUMBO_TAG_SVG:
		{
			this->tag = "SVG";
			break;
		}
		case GUMBO_TAG_FOREIGNOBJECT:
		{
			this->tag = "FOREIGNOBJECT";
			break;
		}
		case GUMBO_TAG_DESC:
		{
			this->tag = "DESC";
			break;
		}
		case GUMBO_TAG_TABLE:
		{
			this->tag = "TABLE";
			break;
		}
		case GUMBO_TAG_CAPTION:
		{
			this->tag = "CAPTION";
			break;
		}
		case GUMBO_TAG_COLGROUP:
		{
			this->tag = "COLGROUP";
			break;
		}
		case GUMBO_TAG_COL:
		{
			this->tag = "COL";
			break;
		}
		case GUMBO_TAG_TBODY:
		{
			this->tag = "TBODY";
			break;
		}
		case GUMBO_TAG_THEAD:
		{
			this->tag = "THEAD";
			break;
		}
		case GUMBO_TAG_TFOOT:
		{
			this->tag = "TFOOT";
			break;
		}
		case GUMBO_TAG_TR:
		{
			this->tag = "TR";
			break;
		}
		case GUMBO_TAG_TD:
		{
			this->tag = "TD";
			break;
		}
		case GUMBO_TAG_TH:
		{
			this->tag = "TH";
			break;
		}
		case GUMBO_TAG_FORM:
		{
			this->tag = "FORM";
			break;
		}
		case GUMBO_TAG_FIELDSET:
		{
			this->tag = "FIELDSET";
			break;
		}
		case GUMBO_TAG_LEGEND:
		{
			this->tag = "LEGEND";
			break;
		}
		case GUMBO_TAG_LABEL:
		{
			this->tag = "LABEL";
			break;
		}
		case GUMBO_TAG_INPUT:
		{
			this->tag = "INPUT";
			break;
		}
		case GUMBO_TAG_BUTTON:
		{
			this->tag = "BUTTON";
			break;
		}
		case GUMBO_TAG_SELECT:
		{
			this->tag = "SELECT";
			break;
		}
		case GUMBO_TAG_DATALIST:
		{
			this->tag = "DATALIST";
			break;
		}
		case GUMBO_TAG_OPTGROUP:
		{
			this->tag = "OPTGROUP";
			break;
		}
		case GUMBO_TAG_OPTION:
		{
			this->tag = "OPTION";
			break;
		}
		case GUMBO_TAG_TEXTAREA:
		{
			this->tag = "TEXTAREA";
			break;
		}
		case GUMBO_TAG_KEYGEN:
		{
			this->tag = "KEYGEN";
			break;
		}
		case GUMBO_TAG_OUTPUT:
		{
			this->tag = "OUTPUT";
			break;
		}
		case GUMBO_TAG_PROGRESS:
		{
			this->tag = "PROGRESS";
			break;
		}
		case GUMBO_TAG_METER:
		{
			this->tag = "METER";
			break;
		}
		case GUMBO_TAG_DETAILS:
		{
			this->tag = "DETAILS";
			break;
		}
		case GUMBO_TAG_SUMMARY:
		{
			this->tag = "SUMMARY";
			break;
		}
		case GUMBO_TAG_MENU:
		{
			this->tag = "MENU";
			break;
		}
		case GUMBO_TAG_MENUITEM:
		{
			this->tag = "MENUITEM";
			break;
		}
		case GUMBO_TAG_APPLET:
		{
			this->tag = "APPLET";
			break;
		}
		case GUMBO_TAG_ACRONYM:
		{
			this->tag = "ACRONYM";
			break;
		}
		case GUMBO_TAG_BGSOUND:
		{
			this->tag = "BGSOUND";
			break;
		}
		case GUMBO_TAG_DIR:
		{
			this->tag = "DIR";
			break;
		}
		case GUMBO_TAG_FRAME:
		{
			this->tag = "FRAME";
			break;
		}
		case GUMBO_TAG_FRAMESET:
		{
			this->tag = "FRAMESET";
			break;
		}
		case GUMBO_TAG_NOFRAMES:
		{
			this->tag = "NOFRAMES";
			break;
		}
		case GUMBO_TAG_ISINDEX:
		{
			this->tag = "ISINDEX";
			break;
		}
		case GUMBO_TAG_LISTING:
		{
			this->tag = "LISTING";
			break;
		}
		case GUMBO_TAG_XMP:
		{
			this->tag = "XMP";
			break;
		}
		case GUMBO_TAG_NEXTID:
		{
			this->tag = "NEXTID";
			break;
		}
		case GUMBO_TAG_NOEMBED:
		{
			this->tag = "NOEMBED";
			break;
		}
		case GUMBO_TAG_PLAINTEXT:
		{
			this->tag = "PLAINTEXT";
			break;
		}
		case GUMBO_TAG_RB:
		{
			this->tag = "RB";
			break;
		}
		case GUMBO_TAG_STRIKE:
		{
			this->tag = "STRIKE";
			break;
		}
		case GUMBO_TAG_BASEFONT:
		{
			this->tag = "BASEFONT";
			break;
		}
		case GUMBO_TAG_BIG:
		{
			this->tag = "BIG";
			break;
		}
		case GUMBO_TAG_BLINK:
		{
			this->tag = "BLINK";
			break;
		}
		case GUMBO_TAG_CENTER:
		{
			this->tag = "CENTER";
			break;
		}
		case GUMBO_TAG_FONT:
		{
			this->tag = "FONT";
			break;
		}
		case GUMBO_TAG_MARQUEE:
		{
			this->tag = "MARQUEE";
			break;
		}
		case GUMBO_TAG_MULTICOL:
		{
			this->tag = "MULTICOL";
			break;
		}
		case GUMBO_TAG_NOBR:
		{
			this->tag = "NOBR";
			break;
		}
		case GUMBO_TAG_SPACER:
		{
			this->tag = "SPACER";
			break;
		}
		case GUMBO_TAG_TT:
		{
			this->tag = "TT";
			break;
		}
		case GUMBO_TAG_RTC:
		{
			this->tag = "RTC";
			break;
		}
		default:
			break;
		}
	}

	bool can_create = false;
	HWND parent = NULL;
	HTMLUI_TypeInfo* typeinfo;
	//HTMLUI_UINode* node;
	std::map<std::string, IUIElement::EventHandler> event_handler;
	int id;

	RECT position() {
		if (parent != NULL) {
			RECT rc;
			GetClientRect(parent, &rc);
			return position(rc);
		}
		// this version of position will treat the right and bottom as the position right and bottom edge
		RECT pos;
		pos.left = left ? left.val() : 0;
		pos.top = top ? top.val() : 0;
		pos.right = right ? right.val() : 30;
		pos.bottom = bottom ? bottom.val() : 30;

		pos.left = x ? x.val() : pos.left;
		pos.top = y ? y.val() : pos.top;
		pos.right = width ? pos.left + width.val() : pos.right;
		pos.bottom = height ? pos.top + height.val() : pos.bottom;

		return pos;
	}
	RECT position(RECT& client_area) {
		// this version of position will treat the right and bottom as the distance of the right and bottom edge to the client area edge.
		RECT pos = client_area;

		pos.left = left ? client_area.left + left.val() : client_area.left;
		pos.top = top ? client_area.top + top.val() : client_area.top;
		pos.right = right ? client_area.right - right.val() : client_area.right;
		pos.bottom = bottom ? client_area.bottom - bottom.val() : client_area.bottom;

		if (!left && x) {
			pos.left = x.val();
		}
		if (!top && y) {
			pos.top = y.val();
		}
		if (left | x && !right && width) {
			pos.right = pos.left + width.val();
		}
		if (top | y && !bottom && height) {
			pos.bottom = pos.top + height.val();
		}
		if (!left && !x && right && width) {
			pos.left = pos.right - width.val();
		}
		if (!top && !y && bottom && height) {
			pos.top = pos.bottom - height.val();
		}
		if (!left && !top && !right && !bottom && !x && !y) {
			pos.right = width ? pos.left + width.val() : pos.right;
			pos.bottom = height ? pos.top + height.val() : pos.bottom;
		}

		return pos;
	}
	std::string identifer(std::string prefix) {
		std::string sp = "::";
		return prefix + sp + name.val() + sp + std::to_string(id);
	}
	std::wstring identifer(std::wstring prefix) {
		std::wstring sp = L"::";
		return prefix + sp + nameW() + sp + std::to_wstring(id);
	}

	Nullable<long> height;
	Nullable<long> width;
	Nullable<long> x;
	Nullable<long> y;

	Nullable<long> left;
	Nullable<long> top;
	Nullable<long> right;
	Nullable<long> bottom;

	Nullable<long> border;

	Nullable<std::string> src;
	Nullable<std::string> name;
	Nullable<std::string> sid;

	std::wstring nameW() {
		if (!name) {
			return L"";
		}
		auto size = (sizeof(wchar_t) * name.val().length() + 1) / sizeof(wchar_t);
		wchar_t* output = new wchar_t[size];
		auto len = MultiByteToWideChar(CP_UTF8, 0, name.val().data(), static_cast<int>(name.val().length()), output, static_cast<int>(size));
		std::wstring name_w(output, len);
		delete output;
		name_w = trim_copy(name_w);
		try {
			extern HINSTANCE hInst;
			name_w = load_resource<std::wstring>(hInst, name_w);
		}
		catch (std::out_of_range) {}
		return name_w;
	}
	std::vector<std::string> classes;
	Nullable<std::string> tag;

};

class IHTMLUI {
public :
	virtual void bind_event_handler(std::string event_name, IUIElement::EventHandler handler) = 0;
	//virtual std::type_index get_type_info() = 0;
};

template <typename UIType>
class HTMLUI : public IHTMLUI {
	friend class HTMLtoUI_Daemon;
public:
	static HTMLUI_TypeInfo::UIConstructor& create_from_html;
	static HTMLUI_TypeInfo::UIMatchAttrMap& match_attributes;
	static HTMLUI_TypeInfo::UISupportedEventsSet& supported_events;
	//virtual std::type_index get_type_info() override
	//{
	//	return typeid(UIType);
	//}
private:
	static void HTMLtoUI_Enable() {
		// make sure the members of the template class are created
		create_from_html;
		match_attributes;
		supported_events;
		type_specified_info;
	}
	static HTMLUI_TypeInfo type_specified_info;
};
template<typename UIType>
HTMLUI_TypeInfo HTMLUI<UIType>::type_specified_info = HTMLUI_TypeInfo(create_from_html, match_attributes, supported_events);
template<typename UIType>
HTMLUI_TypeInfo::UIConstructor& HTMLUI<UIType>::create_from_html = UIType::create_from_html;
template<typename UIType>
HTMLUI_TypeInfo::UIMatchAttrMap& HTMLUI<UIType>::match_attributes = UIType::match_attributes;
template<typename UIType>
HTMLUI_TypeInfo::UISupportedEventsSet& HTMLUI<UIType>::supported_events = UIType::supported_events;

class HTMLUI_UINode
{
public:
	HTMLUI_UINode* parent = NULL;
	std::vector<std::shared_ptr<HTMLUI_UINode>> children;
	std::shared_ptr<UIBase> ui;
	HTMLUI_UIDescriptor descriptor;
};

class HTMLUI_Parser
{
public:
	class named_ui_set : private std::map<std::string, std::shared_ptr<UIBase>> {
		friend class HTMLUI_Parser;
	public:
		mapped_type& query(const key_type& key) {
			return at(key);
		}
	};

	static HTMLUI_UINode parse_file(std::string filename) {
		auto html = std::ifstream(filename);
		std::ostringstream oss;
		oss << html.rdbuf();
		return parse(oss.str());
	}
	static HTMLUI_UINode parse(std::wstring html) {
		auto size = (sizeof(wchar_t) * html.length() + 1) / sizeof(char);
		char* output = new char[size];
		auto len = WideCharToMultiByte(CP_UTF8, 0, html.data(), static_cast<int>(html.length()), output, static_cast<int>(size), NULL, NULL);
		std::string str(output, len);
		delete output;
		return parse(str);
	}
	static HTMLUI_UINode parse(std::string html) {
		auto output = std::shared_ptr<GumboOutput>(gumbo_parse_with_options(&kGumboDefaultOptions, html.c_str(), html.length()), std::bind(&gumbo_destroy_output, &kGumboDefaultOptions, std::placeholders::_1));
		auto docs = output->document;
		HTMLUI_UINode root;
		_parse_node(docs, &root);
		return root;
	}
	static named_ui_set recursive_create(HTMLUI_UINode& node, HWND parent = NULL) {
		named_ui_set ret;
		HWND next_parent = parent;
		// create ui if the node is marked can_create
		if (node.descriptor.can_create) {
			node.descriptor.parent = parent;
			node.ui = node.descriptor.typeinfo->create(node.descriptor);
			for (auto& pair : node.descriptor.event_handler) {
				dynamic_cast<IHTMLUI*>(node.ui.get())->bind_event_handler(pair.first, pair.second);
			}
			next_parent = node.ui->getHandler();

			if (node.descriptor.sid) {
				ret.insert({ node.descriptor.sid.val(), node.ui });
			}
		}

		// create children ui
		for (auto& next : node.children) {
			auto children_ret = HTMLUI_Parser::recursive_create(*(next.get()), next_parent);
			ret.insert(children_ret.begin(), children_ret.end());
		}

		return ret;
	}
private:
	HTMLUI_Parser() {}
	virtual ~HTMLUI_Parser() {}
	static void _parse_node(GumboNode* node, HTMLUI_UINode* current) {
		if (node == nullptr) {
			return;
		}
		// proccess this node
		switch (node->type)
		{
		case GUMBO_NODE_DOCUMENT:
		{
			// do nothing
			break;
		}
		case GUMBO_NODE_ELEMENT:
		{
			current->descriptor.update(node->v.element.tag);
			// Extract css
			switch (node->v.element.tag)
			{
			// external css
			case GUMBO_TAG_LINK:
			{
				std::string href;
				auto rel = gumbo_get_attribute(&(node->v.element.attributes), "rel");
				if (rel && case_insensitive_compare(rel->value, "stylesheet")) {
					auto type = gumbo_get_attribute(&(node->v.element.attributes), "type");
					if (type && case_insensitive_compare(type->value, "text/css")) {
						href = gumbo_get_attribute(&(node->v.element.attributes), "href")->value;
					}
				}
				// TODO: external css
				break;
			}
			// internal css
			case GUMBO_TAG_STYLE:
			{
				// TODO: internal css
				break;
			}
			// inline css
			default:
				auto style = gumbo_get_attribute(&(node->v.element.attributes), "style");
				if (style) {
					auto katana = katana_parse(style->value, strlen(style->value), KatanaParserModeDeclarationList);
					current->descriptor.update(*katana);
					//for (size_t i = 0; i < katana->errors.length; ++i) {
					//	// TODO: log
					//	Alert(static_cast<KatanaError*>(katana->errors.data[i])->message);
					//}
					//if (katana->errors.length > 0) {
					//	Alert("Katana error!");
					//	break;
					//}
					//std::ostringstream oss;
					//oss << std::boolalpha;
					//oss << "StyleSheet Content: \n\t" << style->value << std::endl << std::endl << "Katana Output: \n";
					//for (size_t i = 0; i < katana->declarations->length; ++i) {
					//	auto dec = static_cast<KatanaDeclaration*>(katana->declarations->data[i]);
					//	oss << "\t" << "important: " << dec->important << std::endl;
					//	oss << "\t" << "property: " << dec->property << std::endl;
					//	oss << "\t" << "raw: " << dec->raw << std::endl;
					//	//oss << "\t" << "string: " << dec->string << std::endl;
					//	oss << "\t" << "values: " << std::endl;

					//	for (size_t ii = 0; ii < dec->values->length; ++ii) {
					//		auto val = static_cast<KatanaValue*>(dec->values->data[ii]);
					//		oss << "\t\t" << "index: " << ii << std::endl;
					//		oss << "\t\t" << "id: " << val->id << std::endl;
					//		oss << "\t\t" << "raw: " << val->raw << std::endl;
					//		oss << "\t\t" << "unit: " << val->unit << std::endl;
					//		if (val->isInt) {
					//			oss << "\t\t" << "int value: " << val->iValue << std::endl;
					//		}
					//		oss << "\t\t" << "try int value: " << val->iValue << std::endl;
					//		oss << "\t\t" << "try double value: " << val->fValue << std::endl;
					//		//oss << "\t\t" << "try string value: " << val->string << std::endl;
					//	}
					//	oss << "=======================================" << std::endl;
					//}
					//oss << std::flush;
					//Alert(oss.str());
				}
				break;
			}

			HTMLUI_TypeInfo* target_ui_type_info;
			for (auto& info : HTMLUI_TypeInfo::ui_typeinfo_set) {
				target_ui_type_info = &info;
				for (auto& attr : info.match_attributes) {
					if (case_insensitive_compare("tag", attr.first) && case_insensitive_compare(current->descriptor.tag.val(), attr.second)) {
						//#define match_tag(tag_name, gumbo_tag_value) case gumbo_tag_value: { \
						//	if (case_insensitive_compare( #tag_name , attr.second)) { goto match; \
						//	} \
						//	break; \
						//}
						//						switch (node->v.element.tag)
						//						{
						//							match_tag(HTML, GUMBO_TAG_HTML)
						//							match_tag(HEAD, GUMBO_TAG_HEAD)
						//							match_tag(TITLE, GUMBO_TAG_TITLE)
						//							match_tag(BASE, GUMBO_TAG_BASE)
						//							match_tag(LINK, GUMBO_TAG_LINK)
						//							match_tag(META, GUMBO_TAG_META)
						//							match_tag(STYLE, GUMBO_TAG_STYLE)
						//							match_tag(SCRIPT, GUMBO_TAG_SCRIPT)
						//							match_tag(NOSCRIPT, GUMBO_TAG_NOSCRIPT)
						//							match_tag(TEMPLATE, GUMBO_TAG_TEMPLATE)
						//							match_tag(BODY, GUMBO_TAG_BODY)
						//							match_tag(ARTICLE, GUMBO_TAG_ARTICLE)
						//							match_tag(SECTION, GUMBO_TAG_SECTION)
						//							match_tag(NAV, GUMBO_TAG_NAV)
						//							match_tag(ASIDE, GUMBO_TAG_ASIDE)
						//							match_tag(H1, GUMBO_TAG_H1)
						//							match_tag(H2, GUMBO_TAG_H2)
						//							match_tag(H3, GUMBO_TAG_H3)
						//							match_tag(H4, GUMBO_TAG_H4)
						//							match_tag(H5, GUMBO_TAG_H5)
						//							match_tag(H6, GUMBO_TAG_H6)
						//							match_tag(HGROUP, GUMBO_TAG_HGROUP)
						//							match_tag(HEADER, GUMBO_TAG_HEADER)
						//							match_tag(FOOTER, GUMBO_TAG_FOOTER)
						//							match_tag(ADDRESS, GUMBO_TAG_ADDRESS)
						//							match_tag(P, GUMBO_TAG_P)
						//							match_tag(HR, GUMBO_TAG_HR)
						//							match_tag(PRE, GUMBO_TAG_PRE)
						//							match_tag(BLOCKQUOTE, GUMBO_TAG_BLOCKQUOTE)
						//							match_tag(OL, GUMBO_TAG_OL)
						//							match_tag(UL, GUMBO_TAG_UL)
						//							match_tag(LI, GUMBO_TAG_LI)
						//							match_tag(DL, GUMBO_TAG_DL)
						//							match_tag(DT, GUMBO_TAG_DT)
						//							match_tag(DD, GUMBO_TAG_DD)
						//							match_tag(FIGURE, GUMBO_TAG_FIGURE)
						//							match_tag(FIGCAPTION, GUMBO_TAG_FIGCAPTION)
						//							match_tag(MAIN, GUMBO_TAG_MAIN)
						//							match_tag(DIV, GUMBO_TAG_DIV)
						//							match_tag(A, GUMBO_TAG_A)
						//							match_tag(EM, GUMBO_TAG_EM)
						//							match_tag(STRONG, GUMBO_TAG_STRONG)
						//							match_tag(SMALL, GUMBO_TAG_SMALL)
						//							match_tag(S, GUMBO_TAG_S)
						//							match_tag(CITE, GUMBO_TAG_CITE)
						//							match_tag(Q, GUMBO_TAG_Q)
						//							match_tag(DFN, GUMBO_TAG_DFN)
						//							match_tag(ABBR, GUMBO_TAG_ABBR)
						//							match_tag(DATA, GUMBO_TAG_DATA)
						//							match_tag(TIME, GUMBO_TAG_TIME)
						//							match_tag(CODE, GUMBO_TAG_CODE)
						//							match_tag(VAR, GUMBO_TAG_VAR)
						//							match_tag(SAMP, GUMBO_TAG_SAMP)
						//							match_tag(KBD, GUMBO_TAG_KBD)
						//							match_tag(SUB, GUMBO_TAG_SUB)
						//							match_tag(SUP, GUMBO_TAG_SUP)
						//							match_tag(I, GUMBO_TAG_I)
						//							match_tag(B, GUMBO_TAG_B)
						//							match_tag(U, GUMBO_TAG_U)
						//							match_tag(MARK, GUMBO_TAG_MARK)
						//							match_tag(RUBY, GUMBO_TAG_RUBY)
						//							match_tag(RT, GUMBO_TAG_RT)
						//							match_tag(RP, GUMBO_TAG_RP)
						//							match_tag(BDI, GUMBO_TAG_BDI)
						//							match_tag(BDO, GUMBO_TAG_BDO)
						//							match_tag(SPAN, GUMBO_TAG_SPAN)
						//							match_tag(BR, GUMBO_TAG_BR)
						//							match_tag(WBR, GUMBO_TAG_WBR)
						//							match_tag(INS, GUMBO_TAG_INS)
						//							match_tag(DEL, GUMBO_TAG_DEL)
						//							match_tag(IMAGE, GUMBO_TAG_IMAGE)
						//							match_tag(IMG, GUMBO_TAG_IMG)
						//							match_tag(IFRAME, GUMBO_TAG_IFRAME)
						//							match_tag(EMBED, GUMBO_TAG_EMBED)
						//							match_tag(OBJECT, GUMBO_TAG_OBJECT)
						//							match_tag(PARAM, GUMBO_TAG_PARAM)
						//							match_tag(VIDEO, GUMBO_TAG_VIDEO)
						//							match_tag(AUDIO, GUMBO_TAG_AUDIO)
						//							match_tag(SOURCE, GUMBO_TAG_SOURCE)
						//							match_tag(TRACK, GUMBO_TAG_TRACK)
						//							match_tag(CANVAS, GUMBO_TAG_CANVAS)
						//							match_tag(MAP, GUMBO_TAG_MAP)
						//							match_tag(AREA, GUMBO_TAG_AREA)
						//							match_tag(MATH, GUMBO_TAG_MATH)
						//							match_tag(MI, GUMBO_TAG_MI)
						//							match_tag(MO, GUMBO_TAG_MO)
						//							match_tag(MN, GUMBO_TAG_MN)
						//							match_tag(MS, GUMBO_TAG_MS)
						//							match_tag(MTEXT, GUMBO_TAG_MTEXT)
						//							match_tag(MGLYPH, GUMBO_TAG_MGLYPH)
						//							match_tag(MALIGNMARK, GUMBO_TAG_MALIGNMARK)
						//							match_tag(ANNOTATION_XML, GUMBO_TAG_ANNOTATION_XML)
						//							match_tag(SVG, GUMBO_TAG_SVG)
						//							match_tag(FOREIGNOBJECT, GUMBO_TAG_FOREIGNOBJECT)
						//							match_tag(DESC, GUMBO_TAG_DESC)
						//							match_tag(TABLE, GUMBO_TAG_TABLE)
						//							match_tag(CAPTION, GUMBO_TAG_CAPTION)
						//							match_tag(COLGROUP, GUMBO_TAG_COLGROUP)
						//							match_tag(COL, GUMBO_TAG_COL)
						//							match_tag(TBODY, GUMBO_TAG_TBODY)
						//							match_tag(THEAD, GUMBO_TAG_THEAD)
						//							match_tag(TFOOT, GUMBO_TAG_TFOOT)
						//							match_tag(TR, GUMBO_TAG_TR)
						//							match_tag(TD, GUMBO_TAG_TD)
						//							match_tag(TH, GUMBO_TAG_TH)
						//							match_tag(FORM, GUMBO_TAG_FORM)
						//							match_tag(FIELDSET, GUMBO_TAG_FIELDSET)
						//							match_tag(LEGEND, GUMBO_TAG_LEGEND)
						//							match_tag(LABEL, GUMBO_TAG_LABEL)
						//							match_tag(input, GUMBO_TAG_INPUT)
						//							match_tag(BUTTON, GUMBO_TAG_BUTTON)
						//							match_tag(SELECT, GUMBO_TAG_SELECT)
						//							match_tag(DATALIST, GUMBO_TAG_DATALIST)
						//							match_tag(OPTGROUP, GUMBO_TAG_OPTGROUP)
						//							match_tag(OPTION, GUMBO_TAG_OPTION)
						//							match_tag(TEXTAREA, GUMBO_TAG_TEXTAREA)
						//							match_tag(KEYGEN, GUMBO_TAG_KEYGEN)
						//							match_tag(OUTPUT, GUMBO_TAG_OUTPUT)
						//							match_tag(PROGRESS, GUMBO_TAG_PROGRESS)
						//							match_tag(METER, GUMBO_TAG_METER)
						//							match_tag(DETAILS, GUMBO_TAG_DETAILS)
						//							match_tag(SUMMARY, GUMBO_TAG_SUMMARY)
						//							match_tag(MENU, GUMBO_TAG_MENU)
						//							match_tag(MENUITEM, GUMBO_TAG_MENUITEM)
						//							match_tag(APPLET, GUMBO_TAG_APPLET)
						//							match_tag(ACRONYM, GUMBO_TAG_ACRONYM)
						//							match_tag(BGSOUND, GUMBO_TAG_BGSOUND)
						//							match_tag(DIR, GUMBO_TAG_DIR)
						//							match_tag(FRAME, GUMBO_TAG_FRAME)
						//							match_tag(FRAMESET, GUMBO_TAG_FRAMESET)
						//							match_tag(NOFRAMES, GUMBO_TAG_NOFRAMES)
						//							match_tag(ISINDEX, GUMBO_TAG_ISINDEX)
						//							match_tag(LISTING, GUMBO_TAG_LISTING)
						//							match_tag(XMP, GUMBO_TAG_XMP)
						//							match_tag(NEXTID, GUMBO_TAG_NEXTID)
						//							match_tag(NOEMBED, GUMBO_TAG_NOEMBED)
						//							match_tag(PLAINTEXT, GUMBO_TAG_PLAINTEXT)
						//							match_tag(RB, GUMBO_TAG_RB)
						//							match_tag(STRIKE, GUMBO_TAG_STRIKE)
						//							match_tag(BASEFONT, GUMBO_TAG_BASEFONT)
						//							match_tag(BIG, GUMBO_TAG_BIG)
						//							match_tag(BLINK, GUMBO_TAG_BLINK)
						//							match_tag(CENTER, GUMBO_TAG_CENTER)
						//							match_tag(FONT, GUMBO_TAG_FONT)
						//							match_tag(MARQUEE, GUMBO_TAG_MARQUEE)
						//							match_tag(MULTICOL, GUMBO_TAG_MULTICOL)
						//							match_tag(NOBR, GUMBO_TAG_NOBR)
						//							match_tag(SPACER, GUMBO_TAG_SPACER)
						//							match_tag(TT, GUMBO_TAG_TT)
						//							match_tag(RTC, GUMBO_TAG_RTC)
						//						default:
						//							break;
						//						}
						//#undef match_tag
						goto match;
					}
					auto gumbo_attr = gumbo_get_attribute(&(node->v.element.attributes), attr.first.c_str());
					if (gumbo_attr != NULL && case_insensitive_compare(gumbo_attr->value, attr.second)) {
						goto match;
					}
				}
			}
			break;
		match:
			//current->descriptor.node = current;
			current->descriptor.update(node->v.element.attributes);
			current->descriptor.id = node->v.element.start_pos.offset;
			current->descriptor.typeinfo = target_ui_type_info;
			current->descriptor.can_create = true;

			for (auto& event : target_ui_type_info->supported_events) {
				auto gumbo_attr = gumbo_get_attribute(&(node->v.element.attributes), event.c_str());
				if (gumbo_attr != NULL) {
					current->descriptor.event_handler[event] = target_ui_type_info->get_event_handler(gumbo_attr->value);
				}
			}

			break;
		}
		case GUMBO_NODE_TEXT:
		{
			if (!current->parent->descriptor.name) {
				current->parent->descriptor.name = node->v.text.text;
			}
			auto bak = current->parent->children.back();
			current->parent->children.pop_back();
			return;
		}
		case GUMBO_NODE_CDATA:
		case GUMBO_NODE_COMMENT:
		case GUMBO_NODE_WHITESPACE:
		case GUMBO_NODE_TEMPLATE:
		default:
			// ignore the above types of node
			return;
		}

		// proccess children
		switch (node->type)
		{
		case GUMBO_NODE_DOCUMENT:
		{
			for (size_t i = 0; i < node->v.document.children.length; ++i) {
				current->children.emplace_back(new HTMLUI_UINode);
				current->children.back()->parent = current;
				_parse_node((GumboNode*)node->v.document.children.data[i], current->children.back().get());
			}
			break;
		}
		case GUMBO_NODE_ELEMENT:
		{
			for (size_t i = 0; i < node->v.element.children.length; ++i) {
				current->children.emplace_back(new HTMLUI_UINode);
				current->children.back()->parent = current;
				_parse_node((GumboNode*)node->v.element.children.data[i], current->children.back().get());
			}
			break;
		}
		default:
			return;
		}
	}


};
