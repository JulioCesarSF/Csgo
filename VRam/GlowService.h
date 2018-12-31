#pragma once

#include <mutex>

#include "IService.h"
#include "MemoryManager.h"
#include "Definitions.h"
#include "Offsets.h"

#define GLOW_SIZE				0x38
#define ENTITY_SIZE				90
#define SIZE_TO_READ_GLOW		GLOW_SIZE * ENTITY_SIZE

class GlowService : public IService
{
private:
	std::thread threadRunning;
	Engine::GameHandles game;
	SIGNONSTATE gameStatus = SIGNONSTATE_NONE;
	mutex glowMutex;
public:

	GlowService(Engine::GameHandles game)
	{
		this->game = game;
		SetServiceName("Glow");
	}

	virtual void Start()
	{
		threadRunning = std::thread([&]()
		{
			Run();
		});
		threadRunning.detach();
	}

	inline int ClassId(HANDLE processHandle, DWORD& add)
	{
		DWORD vt = Read<DWORD>(processHandle, (add + 0x8));
		DWORD fn = Read<DWORD>(processHandle, (vt + 2 * 0x4));
		DWORD cls = Read<DWORD>(processHandle, (fn + 0x1));
		int classID = Read<int>(processHandle, (cls + 0x14));
		return classID;
	}

	inline void WriteColor(Vector color, GlowObjectDefinition_t entGlowObject, DWORD toWrite)
	{
		entGlowObject.color.r = color.r;
		entGlowObject.color.g = color.g;
		entGlowObject.color.b = color.b;
		entGlowObject.m_flGlowAlpha = 1.f;
		entGlowObject.m_bRenderWhenOccluded = true;
		entGlowObject.m_bRenderWhenUnoccluded = false;
		::WriteProcessMemory(game.processHandle, (LPVOID)(toWrite), &entGlowObject, sizeof(GlowObjectDefinition_t), NULL);
	}	
	
	inline void WriteColor(Vector color, GlowObjectDefinition entGlowObject, DWORD toWrite, bool isVisible)
	{
		entGlowObject.color.r = color.r;
		entGlowObject.color.g = color.g;
		entGlowObject.color.b = color.b;
		entGlowObject.m_flGlowAlpha = 1.f;
		entGlowObject.m_bRenderWhenOccluded = true;
		entGlowObject.m_bRenderWhenUnoccluded = false;
		entGlowObject.m_bFullBloomRender = false;		
		entGlowObject.m_nStyle = isVisible ? GLOW_STYLE_MODEL : GLOW_STYLE_NORMAL;
		::WriteProcessMemory(game.processHandle, (LPVOID)(toWrite), &entGlowObject, sizeof(GlowObjectDefinition), NULL);
	}

	inline bool IsValidEntity(CBasePlayer player)
	{
		return (player.isDormant() == false && player.getTeam() > 0 && player.getLifeState() == LIFE_ALIVE);
	}
#define GL(AD, I) ((AD + (I * GLOW_SIZE) + 0x4))
#define GL_AD(I)  (I * GLOW_SIZE)
#define GL_OBJ(I) ((I * GLOW_SIZE) + 0x4)
	virtual void Run()
	{
		while (!GetStop())
		{
			Sleep(1);
			if (!isEnabled()) continue;
			if (gameStatus != SIGNONSTATE_FULL) continue;

			const auto glowManager = Read<DWORD>(game.processHandle, (DWORD)game.client + dwGlowObjectManager);
			if (glowManager == 0)
			{
				continue;
			}
			
			BYTE all[SIZE_TO_READ_GLOW];
			if (::ReadProcessMemory(game.processHandle, (LPCVOID)(glowManager), all, sizeof(all), NULL) == FALSE)
			{
				continue;
			}
			for (int i = 0; i <= ENTITY_SIZE && !GetStop(); i++)
			{				
				if (*(DWORD*)(all + GL_AD(i)) == 0) continue;
				
				auto glowEntity = CBasePlayer(game.processHandle, *(DWORD*)(all + GL_AD(i)));
				if (!IsValidEntity(glowEntity)) continue;
				
				auto localPlayer = CBasePlayer(game.processHandle, ((DWORD)game.client + dwLocalPlayer), true);
				if (glowEntity.getId() == localPlayer.getId()) continue;

				const auto classId = ClassId(game.processHandle, *(DWORD*)(all + GL_AD(i)));
				if (classId != CSPlayer) continue;				

				auto entGlowObject = *(GlowObjectDefinition*)(all + GL_OBJ(i));
				
				if (glowEntity.isVisible(localPlayer.getId()))
				{
					WriteColor(Vector(1.0f, 1.0f, 0), entGlowObject, GL(glowManager, i), true);
				}
				else if (glowEntity.getTeam() == localPlayer.getTeam())
				{
					WriteColor(Vector(0, 1.0f, 0), entGlowObject, GL(glowManager, i), false);
				}
				else
				{
					if (glowEntity.isScoped())
					{
						WriteColor(Vector(1.0f, 0, 0.4f), entGlowObject, GL(glowManager, i), false);
					}
					else
					{
						WriteColor(Vector(1.0f, 0.0f, 0.1f), entGlowObject, GL(glowManager, i), false);
					}
				}
			}

		}
		std::cout << "Stopping " << GetServiceName() << std::endl;
		SetCanKill(true);
	}

	virtual void SetGameStatus(SIGNONSTATE gameStatus)
	{
		glowMutex.lock();
		this->gameStatus = gameStatus;
		glowMutex.unlock();
	}
};
