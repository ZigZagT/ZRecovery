#pragma once
#include "stdafx.h"
#include "gumbo.h"
#include "katana.h"
#include "UIBase.h"
#include "HTMLtoUI_Daemon.h"
#include "string_man.h"

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
		Nullable() : is_null(true) {
			if (std::is_default_constructible<T>::value) {
				this->reset(new T());
			}
		}
		template<typename... Types>
		Nullable(Types... args) : std::unique_ptr<T>(new T(std::forward<Types>(args)...)), is_null(false) {}

		Nullable& operator=(T& val) {
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
							c = tolower(c); \
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
	std::wstring nameW() {
		auto size = (sizeof(wchar_t) * name.val().length() + 1) / sizeof(wchar_t);
		wchar_t* output = new wchar_t[size];
		auto len = MultiByteToWideChar(CP_UTF8, 0, name.val().data(), static_cast<int>(name.val().length()), output, static_cast<int>(size));
		std::wstring name_w(output, len);
		delete output;

		return trim_copy(name_w);
	}
	std::vector<std::string> classes;

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
		create_from_html;
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
	static void recursive_create(HTMLUI_UINode& node, HWND parent = NULL) {
		HWND next_parent = parent;
		// create ui if the node is marked can_create
		if (node.descriptor.can_create) {
			node.descriptor.parent = parent;
			node.ui = node.descriptor.typeinfo->create(node.descriptor);
			for (auto& pair : node.descriptor.event_handler) {
				dynamic_cast<IHTMLUI*>(node.ui.get())->bind_event_handler(pair.first, pair.second);
			}
			next_parent = node.ui->getHandler();
		}

		// create children ui
		for (auto& next : node.children) {
			HTMLUI_Parser::recursive_create(*(next.get()), next_parent);
		}
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
					if (case_insensitive_compare("tag", attr.first)) {
#define match_tag(tag_name, gumbo_tag_value) case gumbo_tag_value: { \
	if (case_insensitive_compare( #tag_name , attr.second)) { goto match; \
	} \
	break; \
}
						switch (node->v.element.tag)
						{
							match_tag(button, GUMBO_TAG_BUTTON)
							match_tag(div, GUMBO_TAG_DIV)
							match_tag(p, GUMBO_TAG_P)
						default:
							break;
						}
#undef match_tag
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
			current->parent->descriptor.name = node->v.text.text;
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
