#pragma once

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#include <TlHelp32.h>

#include <string>

using namespace std;

#define TO_ATTACH_OPERATION PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE

static DWORD pid = 0;

template<typename T>
T Read(HANDLE processHandle, DWORD dwAddress)
{
	T buffer = T();
	::ReadProcessMemory(processHandle, (LPCVOID)dwAddress, &buffer, sizeof(buffer), NULL);
	return buffer;
}

namespace Engine
{
	struct GameHandles
	{
		HANDLE processHandle = NULL;
		DWORD client		 = NULL;
		DWORD engine		 = NULL;
	};

	inline void SafeClose(HANDLE handle)
	{
		if (handle == NULL) return;
		::CloseHandle(handle);
	}
	/*
	 * OpenProcess to csgo.exe
	 */
	inline HANDLE Attach(string lpProcessName)
	{
		DWORD processId = 0;
		BOOL next = FALSE;

		HANDLE handle = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (handle == INVALID_HANDLE_VALUE) return NULL;

		PROCESSENTRY32 processentry32;
		processentry32.dwSize = sizeof(PROCESSENTRY32);

		BOOL first = ::Process32First(handle, &processentry32);

		if (!first)
		{
			SafeClose(handle);
			return NULL;
		}

		do
		{
			if (strcmp(processentry32.szExeFile, lpProcessName.c_str()) == 0)
			{
				processId = processentry32.th32ProcessID;
				break;
			}

		} while (next = ::Process32Next(handle, &processentry32));
		SafeClose(handle);

		if (processId == 0)
		{
			return NULL;
		}

		pid = processId;

		handle = ::OpenProcess(TO_ATTACH_OPERATION, FALSE, processId);
		if (handle == (HANDLE)ERROR_ACCESS_DENIED) handle = NULL;

		return handle;
	}
	/*
	 * Get module base
	 */
	inline DWORD GetModuleBaseAddress(DWORD processId, string moduleName)
	{
		if (processId == 0 || moduleName.empty()) return NULL;

		HANDLE handle = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);
		if (handle == INVALID_HANDLE_VALUE) return NULL;

		MODULEENTRY32 moduleentry32;
		moduleentry32.dwSize = sizeof(MODULEENTRY32);

		BOOL first = ::Module32First(handle, &moduleentry32);
		if (!first)
		{
			SafeClose(handle);
			return NULL;
		}

		BOOL next = FALSE;
		DWORD moduleAddress = NULL;
		do
		{
			if (strcmp(moduleentry32.szModule, moduleName.c_str()) == 0)
			{
				moduleAddress = (DWORD)moduleentry32.modBaseAddr;
				break;
			}
		} while (next = ::Module32Next(handle, &moduleentry32));

		Engine::SafeClose(handle);
		return moduleAddress;
	}
}