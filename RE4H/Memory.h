#ifndef __MEMORY__
#define __MEMORY__
#include <string>
#include <type_traits>

using Pointer = char*;
Pointer patternScanHeap(const std::string &unformattedPattern);
Pointer patternScan(const std::string &unformattedPattern);
Pointer patternScan(std::string_view unformattedPattern, std::wstring_view moduleName);

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

template <typename ReturnType = void *>
std::enable_if_t<std::is_pointer_v<ReturnType>, ReturnType> follow(void *instruction)
{
	return reinterpret_cast<ReturnType>(reinterpret_cast<Pointer>(instruction) + getValue<std::int32_t>(reinterpret_cast<Pointer>(instruction) + 1) + 5);
}

template<typename ReturnType, typename ...Args >
std::enable_if_t<std::conjunction_v<std::is_integral<Args>...>, ReturnType*> pointerPath(void *baseAddress, const Args& ...offsets)
{
	return (reinterpret_cast<ReturnType*>(baseAddress = getValue<void*>(reinterpret_cast<Pointer>(baseAddress) + offsets)), ...);
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