#include "Memory.h"

#include <Windows.h>
#include <TlHelp32.h>

#include <algorithm>
#include <vector>
#include <iostream>
#include <iomanip>

namespace
{
	std::string formatPattern(std::string str)
	{
		str.erase(remove(str.begin(), str.end(), 32), str.end());
		std::string result;

		if (str.size() % 2) {
			result.push_back(std::stoi(str.substr(0, 1), nullptr, 16));
			str.erase(str.begin());
		}

		for (std::string::iterator i = str.begin(); i != str.end(); std::advance(i, 2))
		{
			std::string temp(i, next(i, 2));
			if (temp == "??") result.push_back(0x3F);
			else result.push_back(std::stoi(temp, nullptr, 16));
		}

		return result;
	}

	std::vector<PROCESS_HEAP_ENTRY> getHeapElements()
	{
		std::vector<PROCESS_HEAP_ENTRY> result;
		std::vector<HANDLE> heapHandles(GetProcessHeaps(0, nullptr));
		GetProcessHeaps(static_cast<DWORD>(heapHandles.size()), &heapHandles.front());

		for (HANDLE heap : heapHandles)
		{ //for each heap
			PROCESS_HEAP_ENTRY entry;
			entry.lpData = nullptr;

			while (!(HeapWalk(heap, &entry) == FALSE && GetLastError() == ERROR_NO_MORE_ITEMS)) {
				result.push_back(entry);
			}
		}

		return result;
	}

	inline bool compPatternsR(const std::string &pattern, const char *toCompare)
	{
		for (std::string::size_type size = pattern.size(), i = size - 1;; --i)
		{
			if (pattern[i] != '?' && pattern[i] != *(toCompare - (size - 1) + i))
				return false;
			if (!i)
				break;
		}

		return true;
	}

	MODULEENTRY32 getModuleEntry(std::wstring_view mModuleName)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(MODULEENTRY32);
		HANDLE hModuleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());

		if (hModuleSnapshot && Module32First(hModuleSnapshot, &modEntry))
		{
			do {
				if (mModuleName == modEntry.szModule) {
					CloseHandle(hModuleSnapshot);
					return modEntry;
				}
			} while (Module32Next(hModuleSnapshot, &modEntry));

			CloseHandle(hModuleSnapshot);
			throw std::runtime_error("Module not found");
		}

		throw std::runtime_error("Process not found");
	}
}

Pointer patternScanHeap(const std::string &unformattedPattern)
{
	std::string pattern = formatPattern(unformattedPattern);
	DWORD newProtect, oldProtect;

	for (const auto &element : getHeapElements())
	{
		if ((!element.wFlags || element.wFlags & PROCESS_HEAP_REGION || element.wFlags & PROCESS_HEAP_ENTRY_BUSY) && pattern.size() <= element.cbData) //if pattern is longer than block, then pattern can't be here.
		{	
			if (VirtualProtect(element.lpData, element.cbData, PAGE_EXECUTE_READWRITE, &oldProtect)) {

				for (auto i = pattern.size() - 1; i < element.cbData; ++i)
				{
					if (compPatternsR(pattern, (char*)element.lpData + i)) {
						Pointer result = (Pointer)((char*)element.lpData + i - pattern.size() + 1);
						if (result != (Pointer)&pattern.front()) {
							return result;
						}
					}
				}

				VirtualProtect(element.lpData, element.cbData, oldProtect, &newProtect);
			}
			else std::cerr << "VirtualProtect failed at block " << element.lpData << std::endl;
		}
	}

	return 0;
}

Pointer patternScan(const std::string &unformattedPattern)
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	std::string pattern = formatPattern(unformattedPattern);
	DWORD protect;
	Pointer currRegion = (Pointer)sysInfo.lpMinimumApplicationAddress, maxAddress = (Pointer)sysInfo.lpMaximumApplicationAddress;
	MEMORY_BASIC_INFORMATION memInfo;

	while (currRegion < maxAddress)
	{
		VirtualQuery(currRegion, &memInfo, sizeof(MEMORY_BASIC_INFORMATION));
		
		if (memInfo.State == MEM_COMMIT
			&& !(memInfo.Protect & PAGE_GUARD) //don't touch page guard memory
			&& memInfo.RegionSize >= pattern.size()
			&& VirtualProtect(currRegion, memInfo.RegionSize, PAGE_EXECUTE_READWRITE, &protect))
		{
			for (auto i = pattern.size() - 1; i < memInfo.RegionSize; ++i)
			{
				if (compPatternsR(pattern, (Pointer)memInfo.BaseAddress + i))
				{
					Pointer result = (Pointer)memInfo.BaseAddress + i - pattern.size() + 1;
					if (result != pattern.data()) {
						return result;
					}
				}
			}

			VirtualProtect(memInfo.BaseAddress, memInfo.RegionSize, protect, &protect);
		}

		currRegion += memInfo.RegionSize;
	}

	return 0;
}

Pointer patternScan(std::string_view unformattedPattern, std::wstring_view moduleName)
{
	Pointer result = nullptr;
	MODULEENTRY32 moduleEntry;
	std::string pattern;

	if (unformattedPattern.empty())
		return result;

	try
	{
		moduleEntry = getModuleEntry(moduleName);
	}
	catch (const std::runtime_error &)
	{
		return result;
	}

	pattern = formatPattern(std::string(unformattedPattern));

	for (auto i = pattern.size() - 1; i < moduleEntry.modBaseSize; ++i)
	{
		if (compPatternsR(pattern, (Pointer)moduleEntry.modBaseAddr + i))
		{
			Pointer result = (Pointer)moduleEntry.modBaseAddr + i - pattern.size() + 1;
			if (result != pattern.data() && result != unformattedPattern.data()) {
				return result;
			}
		}
	}

	return result;
}