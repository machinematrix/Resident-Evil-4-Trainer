#pragma once
#ifndef __REPORTLISTVIEW__
#define __REPORTLISTVIEW__
#include <Windows.h>
#include <Windowsx.h>
#include <CommCtrl.h>
#include <string>
#pragma comment(lib, "comctl32.lib")

class ReportListView
{
	HMENU wndID = nullptr;
	HWND hWnd = NULL;
	LRESULT(CALLBACK *fnPtr)(HWND, UINT, WPARAM, LPARAM) = nullptr;

public:
	ReportListView() = default;
	ReportListView(int x, int y, int nWidth, int nHeight, DWORD dwStyle, HWND hWndParent, HMENU hMenu, LRESULT(CALLBACK *fnListViewProc)(HWND, UINT, WPARAM, LPARAM) = nullptr);
	ReportListView(const ReportListView &other) = delete;
	ReportListView& operator=(const ReportListView &other) = delete;
	ReportListView& operator=(ReportListView &&other);
	virtual ~ReportListView();
	BOOL addColumn(std::wstring title, unsigned nWidth);
	const HMENU& getID();
	HWND getWindowHandle();
	RECT getRect();
	std::wstring getItemText(int iIndex, int iColumn);
	int findItem(std::wstring text);
	unsigned itemCount();
	void clear();
	void eraseItem(int iIndex);
	void insertItem(int iIndex);
	void insertItem(int iIndex, std::wstring text);
	void insertItemFront();
	void insertItemFront(std::wstring text);
	void insertItemBack();
	void insertItemBack(std::wstring text);
	void setItemText(int iIndex, int iColumn, std::wstring text);
	void setExtendedStyle(DWORD dwExStyle);
	int getSelectedItem();
	void move(int x, int y);
	void resize(int width, int height);
	unsigned getColumnCount();
	void setColumnWidth(int col, int width);
};
#endif