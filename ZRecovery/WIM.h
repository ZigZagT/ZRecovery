#pragma once
class WIM
{
public:
	WIM();
	WIM(std::string filepath);
	virtual ~WIM();
	
	static WIM Open();
	static WIM Open(std::string filepath);

private:
	bool _is_valid;
};

