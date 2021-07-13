#ifndef __MEMORY__
#define __MEMORY__
#include <string>

using Pointer = char*;

Pointer patternScanHeap(const std::string &unformattedPattern);
Pointer patternScan(const std::string &unformattedPattern);
Pointer patternScan(std::string_view unformattedPattern, std::wstring_view moduleName);
void* follow(void *instruction);

template<typename T>
T getValue(void *address)
{
	return *reinterpret_cast<T*>(address);
}

template<typename T>
void setValue(void *address, T value)
{
	*reinterpret_cast<T*>(address) = value;
}

template<typename T, size_t sz>
void setValue(void *address, const T(&value)[sz])
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

template <typename FunctionType>
FunctionType* replaceFunction(void *where, FunctionType *function)
{
	auto result = follow(where);

	setValue(reinterpret_cast<Pointer>(where) + 1, reinterpret_cast<Pointer>(function) - reinterpret_cast<Pointer>(where) - 5);

	return reinterpret_cast<FunctionType*>(result);
}

template <typename T>
T* addBytes(T *base, size_t offset)
{
	return reinterpret_cast<T*>(reinterpret_cast<Pointer>(base) + offset);
}

#endif