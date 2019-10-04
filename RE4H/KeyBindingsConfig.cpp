#include "KeyBindingsConfig.h"
#include <iostream>

const TCHAR *KeyBindingsConfig::configNames[4] = { TEXT("Noclip"), TEXT("Ashley"), TEXT("Heal"), TEXT("Save") };
const TCHAR * const KeyBindingsConfig::sectionName = TEXT("Bindings");

UINT KeyBindingsConfig::getBinding(size_t control) const
{
	UINT result = -1;
	auto strKey = getValue(sectionName, configNames[control]);
	
	if (!strKey.empty()) {
		try { result = std::stoul(strKey); }
		catch(const std::invalid_argument&) {}
	}

	return result;
}

void KeyBindingsConfig::setBinding(size_t control, UINT virtualKeyCode)
{
	setValue(sectionName, configNames[control], std::to_wstring(virtualKeyCode));
}