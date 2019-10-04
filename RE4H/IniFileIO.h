#ifndef __INIFILEIO__
#define __INIFILEIO__
#include <Windows.h>
#include <string>
#include <vector>

class IniFileIO
{
public:
	typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR>> String;
private:
	String file;
	std::vector<String> getXNames(const TCHAR *section, const TCHAR *key) const;
public:
	IniFileIO(const String &file);
	String getValue(const String &section, const String &key) const;
	void setValue(const String &section, const String &key, const String &value);
	void eraseKey(const String &section, const String &key);
	void eraseSection(const String &section);
	std::vector<String> getSectionNames() const;
	std::vector<String> getKeyNames(const String &section) const;
};

#endif