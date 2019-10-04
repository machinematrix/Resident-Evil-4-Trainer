#define _CRT_SECURE_NO_DEPRECATE
#include "Utility.h"

void RedirectSTDIO()
{
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	std::wcout.clear();
	std::cout.clear();
	std::wcerr.clear();
	std::cerr.clear();
	std::wcin.clear();
	std::cin.clear();
}

void ErrorBox(HWND hWnd, const String &text)
{
	MessageBox(hWnd, text.c_str(), TEXT("Error"), MB_ICONERROR);
}

String GetControlText(HWND hWnd)
{
	String result(GetWindowTextLength(hWnd) + 1, L'\0');
	GetWindowText(hWnd, &result.front(), static_cast<int>(result.size()));
	result.pop_back();
	return result;
}

Dimension GetWindowDimensions(HWND hWnd)
{
	RECT rect{ 0 };
	GetWindowRect(hWnd, &rect);
	return Dimension{ rect.right - rect.left, rect.bottom - rect.top };
}

RECT GetWindowRectInParent(HWND hWnd)
{
	RECT result{ 0,0,0,0 };
	GetWindowRect(hWnd, &result);
	MapWindowPoints(HWND_DESKTOP, GetParent(hWnd), (LPPOINT)&result, 2);
	return result;
}

bool CALLBACK SetFont(HWND child, LPARAM font)
{
	SendMessage(child, WM_SETFONT, font, true);
	return true;
}