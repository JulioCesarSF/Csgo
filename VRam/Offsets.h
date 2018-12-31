#pragma once

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>

static DWORD dwLocalPlayer = 0xCBD6B4;
static DWORD dwEntityList = 0x4CCDBFC;
static DWORD m_iHealth = 0x100;
static DWORD m_iTeamNum = 0xF4;
static DWORD m_lifeState = 0x25F;
static DWORD m_bSpottedByMask = 0x980;
static DWORD m_iEntId = 0x64;
static DWORD m_bDormant = 0xED;
static DWORD m_bIsScoped = 0x390A;
static DWORD m_iCrosshairId = 0xB390;
static DWORD m_fFlags = 0x104;
static DWORD m_iShotsFired = 0xA370;

static DWORD dwGlowObjectManager = 0x520DA28;
static DWORD dwClientState = 0x58BCFC;
static DWORD dwClientState_State = 0x108;
static DWORD dwClientState_GetLocalPlayer = 0x180;

static DWORD dwbSendPackets = 0xD210A;

static DWORD dwRadarBase = 0x510293C;

static DWORD dwPlayerResource = 0x30FD65C;
static DWORD dwPlayerResourceName = 0x9D8;