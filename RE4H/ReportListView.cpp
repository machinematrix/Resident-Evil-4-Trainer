#define _CRT_SECURE_NO_WARNINGS
#include "ReportListView.h"

ReportListView::ReportListView(int x, int y, int nWidth, int nHeight, DWORD dwStyle, HWND hWndParent, HMENU hMenu, LRESULT(CALLBACK *fnListViewProc)(HWND, UINT, WPARAM, LPARAM))
	:wndID(hMenu),
	 hWnd(CreateWindow(WC_LISTVIEW, L"", LVS_REPORT | WS_CHILD | dwStyle, x, y, nWidth, nHeight, hWndParent, wndID, NULL, NULL)),
	 fnPtr(fnListViewProc)
{
	if (fnPtr) SetWindowSubclass(hWnd, (SUBCLASSPROC)fnPtr, 1, NULL);
}

ReportListView& ReportListView::operator=(ReportListView &&other)
{
	if (hWnd) DestroyWindow(hWnd);
	hWnd = other.hWnd;
	wndID = other.wndID;
	other.hWnd = NULL;

	return *this;
}

ReportListView::~ReportListView()
{
	if(hWnd) DestroyWindow(hWnd);
}

BOOL ReportListView::addColumn(std::wstring title, unsigned nWidth)
{
	int colCount = getColumnCount();
	BOOL result = TRUE;
	LVCOLUMN lvc;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = colCount ? colCount - 1 : 0;
	lvc.pszText = const_cast<wchar_t*>(title.c_str());
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = nWidth;

	if (SendMessage(hWnd, LVM_INSERTCOLUMN, colCount, (LPARAM)&lvc) == -1) {
		result = FALSE;
	}
	return result;
}

RECT ReportListView::getRect()
{
	RECT lvWndRect;
	GetWindowRect(hWnd,&lvWndRect);
	return lvWndRect;
}

std::wstring ReportListView::getItemText(int iIndex, int iColumn)
{
	std::wstring result(30, L'\0');
	LVITEM item;

	result.reserve(result.size() * 2);
	item.iItem = iIndex;
	item.iSubItem = iColumn;
	item.pszText = nullptr;
	std::wstring::size_type size = 0;

	do {
		result.resize(result.size() * 2);
		std::fill(result.begin(), result.end(), L'\0');
		item.pszText = &result.front();
		item.cchTextMax = static_cast<int>(result.size());
		size = SendMessage(hWnd, LVM_GETITEMTEXT, (WPARAM)iIndex, (LPARAM)&item);
	} while (size >= result.size() - 1);
	result.erase(std::find(result.begin(), result.end(), L'\0'), result.end());
	return result;
}

int ReportListView::findItem(std::wstring text)
{
	LVFINDINFO lvFindInfo;
	lvFindInfo.flags = LVFI_STRING;
	lvFindInfo.psz = text.c_str();
	return ListView_FindItem(hWnd, -1, &lvFindInfo);
}

unsigned ReportListView::itemCount()
{
	return ListView_GetItemCount(hWnd);
}

void ReportListView::clear()
{
	ListView_DeleteAllItems(hWnd);
}

void ReportListView::eraseItem(int iIndex)
{
	ListView_DeleteItem(hWnd, iIndex);
}

void ReportListView::insertItem(int iIndex)
{
	LVITEM newItem;
	newItem.mask = LVIF_STATE;
	newItem.stateMask = 0;
	newItem.state = 0;
	newItem.iIndent = 0;
	newItem.iItem = iIndex;
	newItem.iSubItem = 0;
	ListView_InsertItem(hWnd, &newItem);
}

void ReportListView::insertItem(int iIndex, std::wstring text)
{
	LVITEM newItem;
	newItem.mask = LVIF_STATE | LVIF_TEXT;
	newItem.stateMask = 0;
	newItem.state = 0;
	newItem.iIndent = 0;
	newItem.iItem = iIndex;
	newItem.iSubItem = 0;
	newItem.pszText = &text[0];
	ListView_InsertItem(hWnd, &newItem);
}

void ReportListView::insertItemFront()
{
	LVITEM newItem;
	newItem.mask = LVIF_STATE;
	newItem.stateMask = 0;
	newItem.state = 0;
	newItem.iItem = 0;
	newItem.iSubItem = 0;
	ListView_InsertItem(hWnd, &newItem);
}

void ReportListView::insertItemFront(std::wstring text)
{
	LVITEM newItem;
	newItem.mask = LVIF_STATE | LVIF_TEXT;
	newItem.stateMask = 0;
	newItem.state = 0;
	newItem.iItem = 0;
	newItem.iSubItem = 0;
	newItem.pszText = &text[0];
	ListView_InsertItem(hWnd, &newItem);
}

void ReportListView::insertItemBack()
{
	LVITEM newItem;
	newItem.mask = LVIF_STATE;
	newItem.stateMask = 0;
	newItem.state = 0;
	newItem.iItem = ListView_GetItemCount(hWnd);
	newItem.iSubItem = 0;
	ListView_InsertItem(hWnd, &newItem);
}

void ReportListView::insertItemBack(std::wstring text)
{
	LVITEM newItem;
	newItem.mask = LVIF_STATE | LVIF_TEXT;
	newItem.stateMask = 0;
	newItem.state = 0;
	newItem.iItem = ListView_GetItemCount(hWnd);
	newItem.iSubItem = 0;
	newItem.pszText = &text[0];
	ListView_InsertItem(hWnd, &newItem);
}

int ReportListView::getSelectedItem()
{
	return ListView_GetNextItem(hWnd, -1, LVNI_SELECTED);
}

void ReportListView::move(int x, int y)
{
	RECT rect{0};
	GetWindowRect(hWnd, &rect);
	MoveWindow(hWnd, x, y, rect.right - rect.left, rect.bottom - rect.top, TRUE);
}

void ReportListView::resize(int width, int height)
{
	RECT rect{ 0 };
	GetClientRect(hWnd, &rect);
	MoveWindow(hWnd, rect.left, rect.top, width, height, TRUE);
}

unsigned ReportListView::getColumnCount()
{
	HWND header = (HWND)SendMessage(hWnd, LVM_GETHEADER, 0, 0);
	return static_cast<int>(SendMessage(header, HDM_GETITEMCOUNT, 0, 0));
}

void ReportListView::setColumnWidth(int col, int width)
{
	SendMessage(hWnd, LVM_SETCOLUMNWIDTH, col, width);
}

void ReportListView::setItemText(int iIndex, int iColumn, std::wstring text)
{
	ListView_SetItemText(hWnd, iIndex, iColumn, &text.front());
}

void ReportListView::setExtendedStyle(DWORD dwExStyle)
{
	ListView_SetExtendedListViewStyle(hWnd, dwExStyle);
}

const HMENU& ReportListView::getID()
{
	return wndID;
}

HWND ReportListView::getWindowHandle()
{
	return hWnd;
}