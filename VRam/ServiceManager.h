#pragma once

#include <Windows.h>
#include <vector>
#include <iostream>
#include <mutex>

#include "IService.h"
#include "GlowService.h"
#include "MemoryManager.h"
#include "FakeLagService.h"
#include "GameInfoService.h"
#include "RadarService.h"

class ServiceManager : IService
{
private:
	std::vector<IService*> services;
	Engine::GameHandles game;
	mutex globalMutex;
	GameInfoService* gameInfoService;
	SIGNONSTATE gameStatus = SIGNONSTATE_NONE;
	thread threadRunning;

	void KillAll()
	{
		for (auto service : services)
		{
			while (!service->GetCanKill());
			delete service;
		}		

		Engine::SafeClose(game.processHandle);
	}

	void UpdateGameStatus()
	{
		for (auto service : services)
		{
			globalMutex.lock();
			service->SetGameStatus(gameStatus);
			globalMutex.unlock();
		}
	}

	IService* GetServiceByName(string serviceName)
	{
		for(const auto service : services)
		{
			if(service->GetServiceName() == serviceName)
			{
				return service;
			}
		}

		return nullptr;
	}
public:

	ServiceManager()
	{
		SetServiceName("Service Manager");
	}

	bool Load()
	{
		bool windowFound = false;		
		while(!windowFound)
		{			
			windowFound = ::FindWindow("Valve001", NULL);
			std::cout << "\rWaiting Csgo [" << windowFound << "]" << std::flush;
			this_thread::sleep_for(std::chrono::seconds(2));
			if (::GetAsyncKeyState(VK_DELETE)) return false;
		}
		cout << endl;
		auto handle = Engine::Attach("csgo.exe");
		if (handle == NULL) return false;

		game.processHandle = handle;
		while ( (game.client = Engine::GetModuleBaseAddress(pid, "client_panorama.dll")) == NULL);
		while ( (game.engine = Engine::GetModuleBaseAddress(pid, "engine.dll")) == NULL);

		LoadServices();

		return true;
	}

	void LoadServices()
	{
		gameInfoService = new GameInfoService(game);
		AddService(gameInfoService);
		AddService(new GlowService(game));
		AddService(new FakeLagService(game));
		//AddService(new RadarService(game));
	}

	void AddService(IService* service)
	{
		services.push_back(service);
	}

	void StartAll()
	{
		for (auto service : services)
		{			
			service->Start();
			cout << service->GetServiceName() << " started." << endl;
		}

		this->Start();
	}

	void StopAll()
	{
		this->SetStop(true);

		for (auto service : services)
		{			
			service->SetStop(true);
		}

		KillAll();
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
			
			try
			{
				if (gameInfoService->GetCanKill()) break;
				gameStatus = gameInfoService->GetGameStatus();
			}
			catch (std::exception &e)
			{
				cout << e.what() << endl;
			}
			
			this->UpdateGameStatus();
		}
		std::cout << "Stopping " << GetServiceName() << std::endl;
		SetCanKill(true);
	}
	
	virtual void SetGameStatus(SIGNONSTATE gameStatus)
	{
		this->gameStatus = gameStatus;
	}

	void SetEnabled(bool isEnabled, string serviceName)
	{
		auto service = this->GetServiceByName(serviceName);
		if (service == nullptr) return;
		service->setEnabled(isEnabled);
	}
};
