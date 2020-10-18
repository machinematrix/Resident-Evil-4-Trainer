#ifndef __KEYBINDINGSCONFIG__
#define __KEYBINDINGSCONFIG__
#include "IniFileIO.h"
#include "Bimap.h"

class KeyBindingsConfig : private IniFileIO
{
	static const TCHAR *configNames[9], *const sectionName;
public:
	enum Controls { NOCLIP, ASHLEY, HEAL, SAVE, MERCHANT, MELEE1, MELEE2, INCREASE_ALTITUDE, DECREASE_ALTITUDE };
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