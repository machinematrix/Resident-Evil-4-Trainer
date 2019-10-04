#ifndef __UTILITY__
#define __UTILITY__
#include <windows.h>
#include <string>
#include <iostream>
#include <sstream>

typedef POINT Dimension;
typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR>> String;

void RedirectSTDIO();
void ErrorBox(HWND hWnd, const String &text);
String GetControlText(HWND hWnd);
Dimension GetWindowDimensions(HWND hWnd);
RECT GetWindowRectInParent(HWND hWnd);
bool CALLBACK SetFont(HWND child, LPARAM font);
template <typename T>
String toHexWstring(T number)
{
#ifdef UNICODE
	std::wostringstream ss;
#else
	std::ostringstream ss;
#endif
	ss << std::hex << number;
	return ss.str();
}

#endif