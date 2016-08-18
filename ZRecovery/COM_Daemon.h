#pragma once
class COM_Daemon
{
public:
	~COM_Daemon();
private:
	COM_Daemon();
	static COM_Daemon _start_daemon;
};

