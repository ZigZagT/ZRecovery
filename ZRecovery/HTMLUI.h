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
	static HTMLUI_UINode& Parse(std::wstring html) {
		auto size = (sizeof(wchar_t) * html.length() + 1) / sizeof(char);
		char* output = new char[size];
		auto len = WideCharToMultiByte(CP_UTF8, 0, html.data(), html.length(), output, size, NULL, NULL);
		std::string str(output, len);
		delete output;
		return Parse(str);
	}
	static HTMLUI_UINode& Parse(std::string html) {
		auto output = std::shared_ptr<GumboOutput>(gumbo_parse_with_options(&kGumboDefaultOptions, html.c_str(), html.length()), std::bind(&gumbo_destroy_output, &kGumboDefaultOptions, std::placeholders::_1));
		auto docs = output->document;
		HTMLUI_UINode root;
		_parse_node(docs, &root);
		return root;
	}
	static void recursive_create(HTMLUI_UINode& node, HWND parent = NULL) {
		// find parent hwnd
		if (parent != NULL) {
			node.descriptor.parent = parent;
		}
		else {
			auto parent_node = node.parent;
			while (parent_node != NULL && !parent_node->descriptor.can_create) {
				parent_node = parent_node->parent;
			}
			if (parent_node != NULL) {
				node.descriptor.parent = parent_node->ui->getHandler();
			}
		}

		HWND next_parent = NULL;
		// create ui if the node is marked can_create
		if (node.descriptor.can_create) {
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
		bool skip_current = false;
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
			auto search_set = HTMLUI_TypeInfo::ui_typeinfo_set;
			HTMLUI_TypeInfo* target_ui_type_info;
			for (auto& info : search_set) {
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
			current->parent->descriptor.text = node->v.text.text;
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
				current->children.emplace_back();
				current->children.back()->parent = current;
				_parse_node((GumboNode*)node->v.document.children.data[i], current->children.back().get());
			}
			break;
		}
		case GUMBO_NODE_ELEMENT:
		{
			for (size_t i = 0; i < node->v.element.children.length; ++i) {
				current->children.emplace_back();
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

class HTMLUI_UIDescriptor {
public:
	template<typename T>
	class Nullable : public std::unique_ptr<T> {
	public:
		template<typename... Types>
		Nullable(Types... args) : std::unique_ptr<T>(new T(std::forward<Types>(args)...)) {}

		Nullable& operator=(T& val) {
			this->reset(new T(val));
			return *this;
		}
	};
	HTMLUI_UIDescriptor() {}
	HTMLUI_UIDescriptor(GumboVector& attr_vector) {
		for (int i = 0; i < attr_vector.length; ++i) {
			auto attr = *(GumboAttribute*)attr_vector.data[i];
			auto select = std::bind(HTMLUI_TypeInfo::case_insensitive_compare, attr.name, std::placeholders::_1);
			if (select("width")) {
				width = std::stoi(attr.value);
			}
			else if (select("height")) {
				height = std::stoi(attr.value);
			}
			else if (select("src")) {
				src = attr.value;
			}
		}
	}

	bool can_create = false;
	HWND parent = NULL;
	HTMLUI_TypeInfo* typeinfo;
	std::map<std::string, IUIElement::EventHandler> event_handler;

	Nullable<size_t> height;
	Nullable<size_t> width;
	Nullable<size_t> x;
	Nullable<size_t> y;
	Nullable<size_t> left;
	Nullable<size_t> top;
	Nullable<size_t> right;
	Nullable<size_t> bottom;

	Nullable<std::string> src;
	Nullable<std::string> text;
};