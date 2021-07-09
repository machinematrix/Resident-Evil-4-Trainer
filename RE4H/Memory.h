#ifndef __MEMORY__
#define __MEMORY__
#include <string>

using Pointer = char*;

Pointer patternScanHeap(const std::string &unformattedPattern);
Pointer patternScan(const std::string &unformattedPattern);
Pointer patternScan(std::string_view unformattedPattern, std::wstring_view moduleName);
Pointer follow(Pointer instruction);

template<typename T>
T getValue(Pointer address)
{
	return *reinterpret_cast<T*>(address);
}

template<typename T>
void setValue(Pointer address, T value)
{
	*reinterpret_cast<T*>(address) = value;
}

template<typename T, size_t sz>
void setValue(Pointer address, const T(&value)[sz])
{
	for (size_t i = 0; i < sz; ++i)
		reinterpret_cast<T*>(address)[i] = value[i];
}

template<typename T>
Pointer pointerPath(Pointer baseAddress, const T& offset)
{
	return getValue<Pointer>(baseAddress + offset);
}

template<typename T, typename ...Args>
Pointer pointerPath(Pointer baseAddress, const T& offset, const Args& ...offsets)
{
	return pointerPath(getValue<Pointer>(baseAddress + offset), offsets...);
}

template <typename PointerType>
Pointer replaceFunction(Pointer where, PointerType *function)
{
	auto result = follow(where);

	setValue(where + 1, reinterpret_cast<Pointer>(function) - where - 5);

	return result;
}

template <typename T>
T* addBytes(T *base, size_t offset)
{
	return reinterpret_cast<T*>(reinterpret_cast<Pointer>(base) + offset);
}

#endif