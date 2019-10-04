#include "IniFileIO.h"
#include <sstream>

std::vector<IniFileIO::String> IniFileIO::getXNames(const TCHAR *section, const TCHAR *key) const
{
	String itemsRaw(16, 0), item;
	std::vector<String> result;
	std::basic_istringstream<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR>> items;
	String::size_type copiedChars = GetPrivateProfileString(section, key, nullptr, &itemsRaw.front(), static_cast<DWORD>(itemsRaw.size()), file.c_str());

	while (copiedChars == itemsRaw.size() - 2) {
		itemsRaw.resize(itemsRaw.size() * 2, 0);
		copiedChars = GetPrivateProfileString(section, key, nullptr, &itemsRaw.front(), static_cast<DWORD>(itemsRaw.size()), file.c_str());
	}

	itemsRaw.resize(copiedChars ? copiedChars - 1 : 0);
	items.str(itemsRaw);

	while (std::getline(items, item, String::value_type(0))) {
		result.push_back(item);
	}

	return result;
}

IniFileIO::IniFileIO(const String &file)
	:file(file)
{}

IniFileIO::String IniFileIO::getValue(const String &section, const String &key) const
{
	String result(16, 0);
	
	String::size_type copiedChars = GetPrivateProfileString(section.c_str(), key.c_str(), nullptr, &result.front(), static_cast<DWORD>(result.size()), file.c_str());
	while (copiedChars == result.size() - 1) {
		result.resize(result.size() * 2, 0);
		copiedChars = GetPrivateProfileString(section.c_str(), key.c_str(), nullptr, &result.front(), static_cast<DWORD>(result.size()), file.c_str());
	}
	result.resize(copiedChars);

	return result;
}

void IniFileIO::setValue(const String &section, const String &key, const String &value)
{
	WritePrivateProfileString(section.c_str(), key.c_str(), value.c_str(), file.c_str());
}

void IniFileIO::eraseKey(const String &section, const String &key)
{
	WritePrivateProfileString(section.c_str(), key.c_str(), nullptr, file.c_str());
}

void IniFileIO::eraseSection(const String &section)
{
	WritePrivateProfileString(section.c_str(), nullptr, nullptr, file.c_str());
}

std::vector<IniFileIO::String> IniFileIO::getSectionNames() const
{
	return getXNames(nullptr, nullptr);
}

std::vector<IniFileIO::String> IniFileIO::getKeyNames(const String &section) const
{
	return getXNames(section.c_str(), nullptr);
}