#pragma once

#include <mutex>
#include <random>

#include "IService.h"
#include "MemoryManager.h"
#include "Definitions.h"

class FakeLagService : public IService
{
private:
	std::thread threadRunning;
	Engine::GameHandles game;
	SIGNONSTATE gameStatus = SIGNONSTATE_NONE;
	mutex fakeLagMutex;
public:

	FakeLagService(Engine::GameHandles game)
	{
		this->game = game;
		SetServiceName("FakeLag");
	}

	virtual void Start()
	{
		threadRunning = std::thread([&]()
		{
			Run();
		});
		threadRunning.detach();
	}

	CBasePlayer UpdateLocalPlayer()
	{
		auto localPlayer = CBasePlayer(game.processHandle, ((DWORD)game.client + dwLocalPlayer), true);
		return localPlayer;
	}

	int GetRandomTime(int min, int max)
	{
		//int timeToWait = rand() % (max - min + 1) + min;
		random_device seeder;
		mt19937 engine(seeder());
		uniform_int_distribution<int> generate(min, max);
		return generate(engine);
	}

	virtual void Run()
	{		
		while (!GetStop())
		{
			Sleep(1);
			if (!isEnabled()) continue;
			if (gameStatus != SIGNONSTATE_FULL) continue;
					
			int timeToWait = GetRandomTime(1, 5);
			uint8_t on = 1, off = 0;
			auto sendPacket = Read<BYTE>(game.processHandle, (DWORD)game.engine + dwbSendPackets);			
			if (sendPacket != 0)
			{
				auto localPlayer = UpdateLocalPlayer();				
				if (localPlayer.getMFlag() & FL_ONGROUND && localPlayer.getShotsFired() == 0) 
				{
					::WriteProcessMemory(game.processHandle, (LPVOID)((DWORD)game.engine + dwbSendPackets), &off, sizeof(off), NULL);
					this_thread::sleep_for(chrono::milliseconds(timeToWait));
					::WriteProcessMemory(game.processHandle, (LPVOID)((DWORD)game.engine + dwbSendPackets), &on, sizeof(on), NULL);
				}
			}
		}
		std::cout << "Stopping " << GetServiceName() << std::endl;
		SetCanKill(true);
	}

	virtual void SetGameStatus(SIGNONSTATE gameStatus)
	{
		fakeLagMutex.lock();
		this->gameStatus = gameStatus;
		fakeLagMutex.unlock();
	}
};
