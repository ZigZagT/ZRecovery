#pragma once
#include "stdafx.h"
#include "gumbo.h"
#include "UIBase.h"
#include "HTMLtoUI_Daemon.h"



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
	static bool case_insensitive_compare(std::string s1, std::string s2) {
		if (s1.length() == s2.length()) {
			return std::equal(s1.begin(), s1.end(),
				s2.begin(), [](char a, char b) {
				std::locale loc;
				return std::tolower(a, loc) == std::tolower(b, loc);
			});
		}
		else {
			return false;
		}
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
	HTMLUI_UIDescriptor(GumboVector& attr_vector) {
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
#define add_attr_end() ;

		for (int i = 0; i < attr_vector.length; ++i) {
			auto attr = *(GumboAttribute*)attr_vector.data[i];
			auto select = std::bind(HTMLUI_TypeInfo::case_insensitive_compare, attr.name, std::placeholders::_1);

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
			add_attr_end()
		}

#undef add_attr_start
#undef add_attr_str
#undef add_attr_str2
#undef add_attr_int
#undef add_attr_end
	}
	bool can_create = false;
	HWND parent = NULL;
	HTMLUI_TypeInfo* typeinfo;
	std::map<std::string, IUIElement::EventHandler> event_handler;
	int id;
	RECT position() {
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
	std::string identifer(std::string prefix) {
		std::string sp = "::";
		return prefix + sp + name.val() + sp + std::to_string(id);
	}
	std::wstring identifer(std::wstring prefix) {
		std::wstring sp = L"::";
		return prefix + sp + wide_name() + sp + std::to_wstring(id);
	}

	Nullable<size_t> height;
	Nullable<size_t> width;
	Nullable<size_t> x;
	Nullable<size_t> y;

	Nullable<size_t> left;
	Nullable<size_t> top;
	Nullable<size_t> right;
	Nullable<size_t> bottom;

	Nullable<size_t> border;

	Nullable<std::string> src;
	Nullable<std::string> name;
	std::wstring wide_name() {
		auto size = (sizeof(wchar_t) * name.val().length() + 1) / sizeof(wchar_t);
		wchar_t* output = new wchar_t[size];
		auto len = MultiByteToWideChar(CP_UTF8, 0, name.val().data(), name.val().length(), output, size);
		std::wstring nameW(output, len);
		delete output;

		return nameW;
	}
};

class IHTMLUI {
public :
	virtual void bind_event_handler(std::string event_name, IUIElement::EventHandler handler) = 0;
};

template <typename UIType>
class HTMLUI : public IHTMLUI {
	friend class HTMLtoUI_Daemon;
public:
	static HTMLUI_TypeInfo::UIConstructor& create_from_html;
	static HTMLUI_TypeInfo::UIMatchAttrMap& match_attributes;
	static HTMLUI_TypeInfo::UISupportedEventsSet& supported_events;
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
	static HTMLUI_UINode Parse(std::wstring html) {
		auto size = (sizeof(wchar_t) * html.length() + 1) / sizeof(char);
		char* output = new char[size];
		auto len = WideCharToMultiByte(CP_UTF8, 0, html.data(), html.length(), output, size, NULL, NULL);
		std::string str(output, len);
		delete output;
		return Parse(str);
	}
	static HTMLUI_UINode Parse(std::string html) {
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
		if (current) {
			current->children;
		}
		if (node == nullptr) {
			return;
		}
		// proccess ui
		switch (node->type)
		{
		case GUMBO_NODE_DOCUMENT:
		{
			// do nothing
			break;
		}
		case GUMBO_NODE_ELEMENT:
		{
			HTMLUI_TypeInfo* target_ui_type_info;
			for (auto& info : HTMLUI_TypeInfo::ui_typeinfo_set) {
				for (auto& attr : info.match_attributes) {
					auto gumbo_attr = gumbo_get_attribute(&(node->v.element.attributes), attr.first.c_str());
					if (gumbo_attr != NULL && HTMLUI_TypeInfo::case_insensitive_compare(gumbo_attr->value, attr.second)) {
						target_ui_type_info = &info;
						goto match;
					}
				}
			}
			break;
		match:
			current->descriptor = HTMLUI_UIDescriptor(node->v.element.attributes);
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
