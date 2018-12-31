#pragma once

#include <mutex>

#include "IService.h"
#include "MemoryManager.h"
#include "Offsets.h"
#include "Definitions.h"

class GameInfoService : public IService
{
private:
	std::thread threadRunning;
	Engine::GameHandles game;
	SIGNONSTATE gameStatus = SIGNONSTATE_NONE;
	mutex serviceMutex;
public:

	GameInfoService(Engine::GameHandles game)
	{
		this->game = game;
		SetServiceName("GameInfo");
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
			DWORD enginePointer = Read<DWORD>(game.processHandle, (DWORD)game.engine + dwClientState);			
			SetGameStatus(Read<SIGNONSTATE>(game.processHandle, enginePointer + dwClientState_State));

			if ((::GetAsyncKeyState(VK_DOWN) & 0x0001) != 0)
			{
				DWORD resource = Read<DWORD>(game.processHandle, (DWORD)(game.client + dwPlayerResource));
				for (int i = 3; i < 32; i++)
				{
					char name[32];
					DWORD add;
					::ReadProcessMemory(game.processHandle, (LPCVOID)(resource + dwPlayerResourceName + (i * 4)), &add, sizeof(add), NULL);
					::ReadProcessMemory(game.processHandle, (LPCVOID)(add), &name, sizeof(name), NULL);
					cout << name << endl;
				}
			}
		}
		std::cout << "Stopping " << GetServiceName() << std::endl;
		SetCanKill(true);
	}

	SIGNONSTATE GetGameStatus()
	{
		serviceMutex.lock();
		const auto tempStatus = gameStatus;
		serviceMutex.unlock();
		return tempStatus;
	}

	virtual void SetGameStatus(SIGNONSTATE gameStatus)
	{
		this->gameStatus = gameStatus;
	}
};

