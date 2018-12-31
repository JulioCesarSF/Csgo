#pragma once

#include <Windows.h>

#include "MemoryManager.h"
#include "Offsets.h"
#include <algorithm>

#define SIZE_TO_READ_PLAYER		0x1000 * 11
#define	FL_ONGROUND				(1<<0)	// At rest / on the ground
#define FL_DUCKING				(1<<1)	// Player flag -- Player is fully crouched
#define	FL_WATERJUMP			(1<<3)	// player jumping out of water
#define FL_ONTRAIN				(1<<4) // Player is _controlling_ a train, so movement commands should be ignored on client during prediction.
#define FL_INRAIN				(1<<5)	// Indicates the entity is standing in rain
#define FL_FROZEN				(1<<6) // Player is frozen for 3rd person camera
#define FL_ATCONTROLS			(1<<7) // Player can't move, but keeps key inputs for controlling another entity
#define	FL_CLIENT				(1<<8)	// Is a player
#define FL_FAKECLIENT			(1<<9)	// Fake client, simulated server side; don't send network messages to them
#define	FL_INWATER				(1<<10)	// In water

enum LifeState
{
	LIFE_ALIVE = 0,
	LIFE_DYING = 1,
	LIFE_DEAD = 2
};

enum ENTITY_TYPE
{
	CSPlayer = 38
};

enum SIGNONSTATE
{
	SIGNONSTATE_NONE = 0,
	SIGNONSTATE_CHALLENGE = 1,
	SIGNONSTATE_CONNECTED = 2,
	SIGNONSTATE_NEW = 3,
	SIGNONSTATE_PRESPAWN = 4,
	SIGNONSTATE_SPAWN = 5,
	SIGNONSTATE_FULL = 6,
	SIGNONSTATE_CHANGELEVEL = 7
};

struct Vector
{
	float r = 0;
	float g = 0;
	float b = 0;
	Vector(){}
	/*Vector(int r, int g, int b)
	{
		this->r = 1.0f / r;
		this->g = 1.0f / r;
		this->b = 1.0f / r;
	}*/
	Vector(float r, float g, float b)
	{
		this->r = (float)max(0, min(255, (int)floor(r * 256.0)));
		this->g = (float)max(0, min(255, (int)floor(g * 256.0)));
		this->b = (float)max(0, min(255, (int)floor(b * 256.0)));
	}
};

struct GlowObjectDefinition_t
{
	Vector color;
	float m_flGlowAlpha;
	char m_unk[4];
	float m_flUnk;
	float m_flBloomAmount;
	float m_flUnk1;
	bool m_bRenderWhenOccluded;
	bool m_bRenderWhenUnoccluded;
	bool m_bFullBloomRender;
};

enum GlowStyle : int
{
	GLOW_STYLE_NORMAL = 0,
	GLOW_STYLE_MODEL,
	GLOW_STYLE_INLINE,
	GLOW_STYLE_INLINE_PULSATE,
	GLOW_STYLE_MAX
};

class GlowObjectDefinition
{
	// some code removed
public:
	Vector color;
	float m_flGlowAlpha;
private:
	char unknown[4];
public:
	float m_flUnk;
	float m_flBloomAmount;
	float m_flLocalPlayerIsZeroPoint;
public:
	bool m_bRenderWhenOccluded;
	bool m_bRenderWhenUnoccluded;
	bool m_bFullBloomRender;
private:
	char unknown1[1];
public:
	int m_nFullBloomStencilTestValue;
	GlowStyle m_nStyle;
};

struct CBasePlayer
{
	BYTE entity[SIZE_TO_READ_PLAYER] = { 0 };
	CBasePlayer(const HANDLE processHandle, DWORD toRead, bool isLocalPlayer = false)
	{
		if (isLocalPlayer)
		{
			const DWORD localPlayer = Read<DWORD>(processHandle, toRead);
			auto out = &localPlayer;
			::ReadProcessMemory(processHandle, (LPCVOID)localPlayer, entity, sizeof(entity), NULL);
		}
		else
		{
			::ReadProcessMemory(processHandle, (LPCVOID)toRead, entity, sizeof(entity), NULL);
		}
	}

	int		getHealth() { return *(int*)(entity + m_iHealth); }
	int		getTeam() { return *(int*)(entity + m_iTeamNum); }
	int		getLifeState() { return *(LifeState*)(entity + m_lifeState); }
	int		getId() { return *(int*)(entity + m_iEntId); }
	bool	isVisible(int entityId) { return (bool)(*(INT64*)(entity + m_bSpottedByMask) & (1i64 << (entityId - 1))); }
	bool	isDormant() { return *(bool*)(entity + m_bDormant); }
	bool	isScoped() { return *(bool*)(entity + m_bIsScoped) == 1; }
	BYTE	getMFlag() { return *(BYTE*)(entity + m_fFlags); }
	int		getShotsFired() { return *(int*)(entity + m_iShotsFired); }

	void	dump()
	{
		cout << "Health[" << getHealth() << "] Team[" << getTeam() << "] LifeState[" << getLifeState()
			<< "] Id[" << getId() << "] Dormant[" << isDormant() << "] isScoped[" << isScoped()
			<< "] Flag[" << getMFlag() << "] ShotsFired[" << getShotsFired() << "]" << endl;
	}
};