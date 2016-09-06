#pragma once
#include "stdafx.h"

template <typename COM_Interface>
class COM_Proxy
{
public:
	COM_Proxy() :
		_is_valid(false),
		_com_interface(nullptr)
	{
	}

	COM_Proxy(COM_Interface* com_interface) :
		_is_valid(true),
		_com_interface(com_interface)
	{
		if (com_interface == nullptr) {
			_is_valid = false;
		}
	}

	operator COM_Interface*() {
		return get();
	}

	operator COM_Interface**() {
		return pget();
	}

	COM_Interface* operator->() {
		if (!_is_valid) {
			throw std::runtime_error("invalid interface");
		}
		return _com_interface;
	}

	COM_Interface* get() const
	{
		return _com_interface;
	}

	COM_Interface** pget() const
	{
		return &const_cast<COM_Interface*>(_com_interface);
	}

	void active() {
		_is_valid = true;
	}

	void reset(COM_Interface* com_interface = nullptr)
	{
		if (_is_valid) {
			_com_interface->Release();
		}
		_is_valid = true;
		_com_interface = com_interface;
		if (com_interface == nullptr) {
			_is_valid = false;
		}
	}

	virtual ~COM_Proxy()
	{
		if (_is_valid) {
			_com_interface->Release();
		}
	}

private:
	bool _is_valid;
	COM_Interface* _com_interface;
};

template <typename COM_Interface_t>
std::shared_ptr<COM_Interface_t> COM_make_shared(COM_Interface_t* com_interface) {
	return std::shared_ptr<COM_Interface_t>(com_interface, [](COM_Interface_t* ptr) => { ptr->Release();  });
}

