#pragma once

#include <mutex>

#include "IService.h"
#include "MemoryManager.h"
#include "Definitions.h"

class RadarService : public IService
{
private:
	std::thread threadRunning;
	Engine::GameHandles game;
	SIGNONSTATE gameStatus = SIGNONSTATE_NONE;
	mutex radarMutex;
public:

	RadarService(Engine::GameHandles game)
	{
		this->game = game;
		SetServiceName("Radar");
	}

	virtual void Start()
	{
		threadRunning = std::thread([&]()
		{
			Run();
		});
		threadRunning.detach();
	}

	virtual void Run()
	{
		while (!GetStop())
		{
			Sleep(1);
			if (!isEnabled()) continue;
			if (gameStatus != SIGNONSTATE_FULL) continue;

			

		}
		std::cout << "Stopping " << GetServiceName() << std::endl;
		SetCanKill(true);
	}

	virtual void SetGameStatus(SIGNONSTATE gameStatus)
	{
		radarMutex.lock();
		this->gameStatus = gameStatus;
		radarMutex.unlock();
	}
};
