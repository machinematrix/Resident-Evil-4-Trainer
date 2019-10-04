#ifndef __KEYBINDINGSCONFIG__
#define __KEYBINDINGSCONFIG__
#include "IniFileIO.h"
#include "Bimap.h"

class KeyBindingsConfig : private IniFileIO
{
	static const TCHAR *configNames[4], *const sectionName;
public:
	enum Controls { Noclip, Ashley, Heal, Save };
	//HotkeyManager(const String &file, HINSTANCE hModule, DWORD threadId);
	using IniFileIO::IniFileIO;
	UINT getBinding(size_t control) const;
	void setBinding(size_t control, UINT virtualKeyCode);
	static constexpr size_t getControlCount();
};

constexpr size_t KeyBindingsConfig::getControlCount()
{
	return sizeof(configNames) / sizeof(TCHAR*);
}

#endif