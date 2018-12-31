#pragma once

#include <thread>
#include <string>

#include "Definitions.h"

class IService
{
private:
	bool stop		= false;
	bool canKill	= false;
	bool enabled	= true;
	std::string		serviceName;
public:
	void setEnabled(bool isEnabled) { enabled = isEnabled; }
	bool isEnabled() { return enabled; }
	void SetStop(bool value) { stop = value; }
	bool GetStop() { return stop; }
	void SetCanKill(bool value) { canKill = value; }
	bool GetCanKill() { return canKill; }
	void SetServiceName(std::string value) { serviceName = value; }
	std::string GetServiceName() { return serviceName; }
	void Sleep(int ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }
	virtual ~IService() {}
	virtual void Start() = 0;
	virtual void Run() = 0;
	virtual void SetGameStatus(SIGNONSTATE gameStatus) = 0;
};