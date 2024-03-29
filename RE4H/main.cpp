#include <Windows.h>
#include <CommCtrl.h>
#include <psapi.h>

#include <vector>
#include <tuple>
#include <array>
#include <string_view>
#include "Handlers.h"
#include "Utility.h"
#include "Features.h"
#include "InventoryListView.h"
#include "KeyBindingsConfig.h"
#include "resource.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

DWORD WINAPI ThreadProc(LPVOID);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	BOOL result = TRUE;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		using namespace std::string_view_literals;
		std::array<char, MAX_PATH> fileNameBuffer = {};
		GetModuleBaseNameA(GetCurrentProcess(), nullptr, fileNameBuffer.data(), fileNameBuffer.size());

		if (std::string_view { fileNameBuffer.data() } == "bio4.exe"sv)
		{
			if (HANDLE hThread = CreateThread(nullptr, 0, ThreadProc, hModule, 0, nullptr))
				CloseHandle(hThread);
			else
			{
				ErrorBox(nullptr, TEXT("Could not create thread"));
				result = FALSE;
			}
		}
		else
			result = FALSE;
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return result;
}

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	WNDCLASSEXW wcex;
	MSG msg;
	const TCHAR *windowClass = TEXT("MainWnd"), *title = TEXT("RE4H");
	const int horSize = 640, verSize = 540;
	DWORD exitCode = -1;

#ifndef NDEBUG
	AllocConsole();
	RedirectSTDIO();
#endif

	if (Features::Initialize())
	{
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = static_cast<HINSTANCE>(lpParameter);
		wcex.hIcon = 0;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = windowClass;
		wcex.hIconSm = 0;
		RegisterClassEx(&wcex); //Register main window

		HMENU hMenu = CreateMenu(), hMenuPopup = CreateMenu();

		AppendMenu(hMenuPopup, MF_STRING, MenuIdentifiers::KEY_BINDINGS, TEXT("Key bindings"));
		AppendMenu(hMenuPopup, MF_SEPARATOR, 0, nullptr);
		AppendMenu(hMenuPopup, MF_STRING, MenuIdentifiers::EXIT, TEXT("Exit"));
		AppendMenu(hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hMenuPopup), TEXT("File"));

		if (HWND hWnd = CreateWindowW(windowClass, title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, horSize, verSize, nullptr, hMenu, (HINSTANCE)lpParameter, nullptr))
		{
			ShowWindow(hWnd, SW_SHOW);
			UpdateWindow(hWnd);

			while (GetMessage(&msg, nullptr, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			exitCode = msg.wParam;
		}
		else
			ErrorBox(nullptr, TEXT("Could not create main window"));

		UnregisterClass(windowClass, static_cast<HMODULE>(lpParameter));
		Features::Terminate();
	}
	else
		ErrorBox(nullptr, TEXT("Could not find one or more pointers"));
	
#ifndef NDEBUG
	FreeConsole();
#endif
	FreeLibraryAndExitThread(static_cast<HMODULE>(lpParameter), exitCode);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static MainWindowInfo wndInfo;

	switch (message)
	{
	case WM_CREATE:
		onWmCreate(hWnd, wParam, lParam, wndInfo);
		break;

	case WM_SIZE:
		onWmSize(hWnd, wParam, lParam, wndInfo);
		break;

	case WM_TIMER:
		onWmTimer(hWnd, wParam, lParam, wndInfo);
		break;

	case WM_COMMAND:
		onWmCommand(hWnd, wParam, lParam, wndInfo);
		break;

	case WM_NOTIFY:
		onWmNotify(hWnd, wParam, lParam, wndInfo);
		break;

	case WM_KEYDOWN:
		onWmKeydown(hWnd, wParam, lParam, wndInfo);
		break;

	case WM_DESTROY:
		onWmDestroy(hWnd, wParam, lParam, wndInfo);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

BOOL CALLBACK EditMaxAmountDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND itemCombo, amountEdit, enableCheck;
	static int itemComboIdentifier;

	switch (message)
	{
		case WM_INITDIALOG:
			itemCombo = GetDlgItem(hDlg, IDC_ITEM_MAX_AMOUNT_COMBO);
			amountEdit = GetDlgItem(hDlg, IDC_ITEM_MAX_AMOUNT_EDIT);
			enableCheck = GetDlgItem(hDlg, IDC_ENABLESTACKS);
			itemComboIdentifier = GetDlgCtrlID(itemCombo);

			for (const auto &name : Features::GetItemNames())
				SendMessage(itemCombo, CB_ADDSTRING, 0, (LPARAM)name.c_str());

			SendMessage(enableCheck, BM_SETCHECK, static_cast<WPARAM>(Features::IsMaxItemHookEnabled()), 0);
			break;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDCANCEL:
					EndDialog(hDlg, 0);
					break;
				case IDC_ITEM_MAX_AMOUNT_COMBO:
					if (HIWORD(wParam) == CBN_SELCHANGE)
					{
						std::int16_t id = static_cast<std::int16_t>(SendMessage(itemCombo, CB_GETCURSEL, 0, 0));
						if (id != CB_ERR) {
							std::wstring strAmount = std::to_wstring(Features::GetItemDimensions(static_cast<Features::ItemId>(id)).stackLimit());
							String strAmount2(strAmount.begin(), strAmount.end());
							SetWindowText(amountEdit, strAmount.c_str());
						}
					}
					break;

				case IDC_SETAMOUNT:
				{
					std::int16_t id = static_cast<std::int16_t>(SendMessage(itemCombo, CB_GETCURSEL, 0, 0));

					if (id != CB_ERR)
					{
						try {
							Features::SetMaxItemAmount(static_cast<Features::ItemId>(id), std::stoul(GetControlText(amountEdit))); //not checking whether the text is a valid number, but it doesn't matter since the edit control is number-only
						}
						catch (const std::invalid_argument &) {
							ErrorBox(hDlg, TEXT("Invalid capacity"));
						}
					}
					break;
				}
				case IDC_ENABLESTACKS:
					Features::ToggleMaxItemAmountHook(SendMessage(enableCheck, BM_GETCHECK, 0, 0) == BST_CHECKED ? true : false);
					break;
			}

			break;
		}
	}

	return FALSE;
}

BOOL CALLBACK ItemDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND itemCombo, inventoryCombo, amountEdit, firepowerCombo, firingSpeedCombo, reloadSpeedCombo, capacityCombo, ammoEdit, posXEdit, posYEdit, rotationCombo;
	static Features::ItemData *item;

	switch (message)
	{
		case WM_INITDIALOG:
		{
			item = reinterpret_cast<Features::ItemData*>(lParam);

			itemCombo = GetDlgItem(hDlg, IDC_ITEM_COMBO);
			inventoryCombo = GetDlgItem(hDlg, IDC_INVENTORY_COMBO);
			amountEdit = GetDlgItem(hDlg, IDC_AMOUNT_EDIT);
			firepowerCombo = GetDlgItem(hDlg, IDC_FIREPOWER_COMBO);
			firingSpeedCombo = GetDlgItem(hDlg, IDC_FIRING_SPEED_COMBO);
			reloadSpeedCombo = GetDlgItem(hDlg, IDC_RELOAD_SPEED_COMBO);
			capacityCombo = GetDlgItem(hDlg, IDC_CAPACITY_COMBO);
			ammoEdit = GetDlgItem(hDlg, IDC_AMMO_EDIT);
			posXEdit = GetDlgItem(hDlg, IDC_POSITION_X_EDIT);
			posYEdit = GetDlgItem(hDlg, IDC_POSITION_Y_EDIT);
			rotationCombo = GetDlgItem(hDlg, IDC_ROTATION_COMBO);

			for (const auto &name : Features::GetItemNames())
				SendMessage(itemCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(name.c_str()));

			for (int i = 0; i < 2; ++i)
				SendMessage(inventoryCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(std::to_wstring(i).c_str()));

			for (unsigned i = 0; i < 7; ++i)
			{
				TCHAR str[2] = { (TCHAR)(TEXT('0') + i), 0 };
				SendMessage(firepowerCombo, CB_ADDSTRING, 0, (LPARAM)str);
				SendMessage(firingSpeedCombo, CB_ADDSTRING, 0, (LPARAM)str);
				SendMessage(reloadSpeedCombo, CB_ADDSTRING, 0, (LPARAM)str);
				SendMessage(capacityCombo, CB_ADDSTRING, 0, (LPARAM)str);
			}

			SendMessage(rotationCombo, CB_ADDSTRING, 0, (LPARAM)TEXT("Right"));
			SendMessage(rotationCombo, CB_ADDSTRING, 0, (LPARAM)TEXT("Down"));
			SendMessage(rotationCombo, CB_ADDSTRING, 0, (LPARAM)TEXT("Left"));
			SendMessage(rotationCombo, CB_ADDSTRING, 0, (LPARAM)TEXT("Up"));

			SendMessage(itemCombo, CB_SETCURSEL, static_cast<WPARAM>(item->itemId()), 0);
			SendMessage(inventoryCombo, CB_SETCURSEL, static_cast<WPARAM>(item->inventory()), 0);
			SetWindowText(amountEdit, std::to_wstring(item->amount()).c_str());
			SendMessage(firepowerCombo, CB_SETCURSEL, item->firePower(), 0);
			SendMessage(firingSpeedCombo, CB_SETCURSEL, item->firingSpeed(), 0);
			SendMessage(reloadSpeedCombo, CB_SETCURSEL, item->reloadSpeed(), 0);
			SendMessage(capacityCombo, CB_SETCURSEL, item->capacity(), 0);
			SetWindowText(ammoEdit, std::to_wstring(item->ammo()).c_str());
			SetWindowText(posXEdit, std::to_wstring(item->posX()).c_str());
			SetWindowText(posYEdit, std::to_wstring(item->posY()).c_str());
			SendMessage(rotationCombo, CB_SETCURSEL, item->rotation(), 0);
			break;
		}
		case WM_COMMAND:
		{
			int valid = 0;

			switch (LOWORD(wParam))
			{
				case IDOK:
				{
					std::uint16_t id = static_cast<std::uint16_t>(SendMessage(itemCombo, CB_GETCURSEL, 0, 0)), amount = 0, fp = 0, fs = 0, rs = 0, ca = 0, ammo = 0, x = 0, y = 0, rotation = 0;
					std::uint8_t inventory = 0;

					if (id == CB_ERR) { //ID
						ErrorBox(hDlg, TEXT("Select an item"));
						break;
					}

					try {
						amount = std::stoi(GetControlText(amountEdit)); //AMOUNT
					}
					catch (const std::invalid_argument&) {
						ErrorBox(hDlg, TEXT("Invalid amount"));
						break;
					}

					if ((inventory = ComboBox_GetCurSel(inventoryCombo)) == CB_ERR) {
						ErrorBox(hDlg, TEXT("Select an inventory"));
						break;
					}

					if ((fp = ComboBox_GetCurSel(firepowerCombo)) == CB_ERR) { //FP
						ErrorBox(hDlg, TEXT("Select a fire power level"));
						break;
					}

					if ((fs = ComboBox_GetCurSel(firingSpeedCombo)) == CB_ERR) { //FS
						ErrorBox(hDlg, TEXT("Select a firing speed level"));
						break;
					}

					if ((rs = ComboBox_GetCurSel(reloadSpeedCombo)) == CB_ERR) { //RS
						ErrorBox(hDlg, TEXT("Select a reload speed level"));
						break;
					}

					if ((ca = ComboBox_GetCurSel(capacityCombo)) == CB_ERR) { //CA
						ErrorBox(hDlg, TEXT("Select a capacity level"));
						break;
					}

					try {
						ammo = std::stoi(GetControlText(ammoEdit)); //AMMO
					}
					catch (const std::invalid_argument&) {
						ErrorBox(hDlg, TEXT("Invalid ammo amount"));
						break;
					}

					try {
						x = std::stoi(GetControlText(posXEdit)); //X
					}
					catch (const std::invalid_argument&) {
						ErrorBox(hDlg, TEXT("Invalid horizontal position"));
						break;
					}

					try {
						y = std::stoi(GetControlText(posYEdit)); //Y
					}
					catch (const std::invalid_argument&) {
						ErrorBox(hDlg, TEXT("Invalid vertical position"));
						break;
					}

					if ((rotation = ComboBox_GetCurSel(rotationCombo)) == CB_ERR) { //ROTATION
						ErrorBox(hDlg, TEXT("Select an orientation"));
						break;
					}

					item->itemId(static_cast<Features::ItemId>(id));
					item->amount(amount);
					item->inventory(inventory);
					item->firePower(fp);
					item->firingSpeed(fs);
					item->reloadSpeed(rs);
					item->capacity(ca);
					item->ammo(ammo);
					item->posX(static_cast<std::uint8_t>(x));
					item->posY(static_cast<std::uint8_t>(y));
					item->rotation(static_cast<std::uint8_t>(rotation));
					valid = 1;
				}
				[[fallthrough]];
				case IDCANCEL:
					item = nullptr;
					EndDialog(hDlg, valid);
					break;
			}
		}
	}

	return FALSE;
}

BOOL CALLBACK WeaponDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND firePowerEdits[7], firingSpeedEdits[5], capacityEdits[7], modelEdit, ammoCombo;
	static std::vector<Features::ItemId> ids;
	static Features::WeaponData *data;

	switch (message)
	{
		case WM_INITDIALOG:
		{
			data = reinterpret_cast<Features::WeaponData*>(lParam);

			if (!data)
			{
				ErrorBox(hDlg, TEXT("Null weapon data pointer"));
				EndDialog(hDlg, 0);
				break;
			}

			firePowerEdits[0] = GetDlgItem(hDlg, IDC_FIREPOWER_LV_1);
			firePowerEdits[1] = GetDlgItem(hDlg, IDC_FIREPOWER_LV_2);
			firePowerEdits[2] = GetDlgItem(hDlg, IDC_FIREPOWER_LV_3);
			firePowerEdits[3] = GetDlgItem(hDlg, IDC_FIREPOWER_LV_4);
			firePowerEdits[4] = GetDlgItem(hDlg, IDC_FIREPOWER_LV_5);
			firePowerEdits[5] = GetDlgItem(hDlg, IDC_FIREPOWER_LV_6);
			firePowerEdits[6] = GetDlgItem(hDlg, IDC_FIREPOWER_LV_7);
			firingSpeedEdits[0] = GetDlgItem(hDlg, IDC_FIRINGSPEED_LV_1);
			firingSpeedEdits[1] = GetDlgItem(hDlg, IDC_FIRINGSPEED_LV_2);
			firingSpeedEdits[2] = GetDlgItem(hDlg, IDC_FIRINGSPEED_LV_3);
			firingSpeedEdits[3] = GetDlgItem(hDlg, IDC_FIRINGSPEED_LV_4);
			firingSpeedEdits[4] = GetDlgItem(hDlg, IDC_FIRINGSPEED_LV_5);
			capacityEdits[0] = GetDlgItem(hDlg, IDC_CAPACITY_LV_1);
			capacityEdits[1] = GetDlgItem(hDlg, IDC_CAPACITY_LV_2);
			capacityEdits[2] = GetDlgItem(hDlg, IDC_CAPACITY_LV_3);
			capacityEdits[3] = GetDlgItem(hDlg, IDC_CAPACITY_LV_4);
			capacityEdits[4] = GetDlgItem(hDlg, IDC_CAPACITY_LV_5);
			capacityEdits[5] = GetDlgItem(hDlg, IDC_CAPACITY_LV_6);
			capacityEdits[6] = GetDlgItem(hDlg, IDC_CAPACITY_LV_7);
			modelEdit = GetDlgItem(hDlg, IDC_MODEL_EDIT);
			ammoCombo = GetDlgItem(hDlg, IDC_AMMO_TYPE_COMBO);

			auto &firePowerEntry = Features::GetFirePowerTableEntry(data->statIndex());
			auto &firingSpeedEntry = Features::GetFiringSpeedTableEntry(data->statIndex());;
			for (size_t i = 0; i < 7; ++i)
			{
				SetWindowText(firePowerEdits[i], std::to_wstring(firePowerEntry[i]).c_str());
				if (i < 5)
					SetWindowText(firingSpeedEdits[i], std::to_wstring(firingSpeedEntry[i]).c_str());
				SetWindowText(capacityEdits[i], std::to_wstring(data->capacity(i)).c_str());
			}
			SetWindowText(modelEdit, std::to_wstring(data->model()).c_str());
			for (const auto &ammoId : Features::GetAmmoItemIds())
			{
				SendMessage(ammoCombo, CB_ADDSTRING, 0, (LPARAM)Features::GetItemName(ammoId).c_str());
				ids.push_back(ammoId);
				if (data->weaponAmmo() == ammoId)
					SendMessage(ammoCombo, CB_SETCURSEL, SendMessage(ammoCombo, CB_GETCOUNT, 0, 0) - 1, 0);
			}
			break;
		}

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
				{
					Features::WeaponData newData = *data;
					float newFirepower[7], newFiringSpeed[5];

					try
					{
						for (size_t i = 0; i < 7; ++i)
						{
							newFirepower[i] = std::stof(GetControlText(firePowerEdits[i]));
							if (i < 5)
								newFiringSpeed[i] = std::stof(GetControlText(firingSpeedEdits[i]));
							newData.capacity(i, std::stoi(GetControlText(capacityEdits[i])));
						}
					}
					catch (const std::invalid_argument&)
					{
						ErrorBox(hDlg, TEXT("Invalid value in fire power and/or capacity"));
						break;
					}

					try
					{
						newData.model(std::stoi(GetControlText(modelEdit)));
					}
					catch (const std::invalid_argument&)
					{
						ErrorBox(hDlg, TEXT("Invalid value in model"));
						break;
					}

					try
					{
						if (auto curSel = ComboBox_GetCurSel(ammoCombo); curSel != CB_ERR)
						{
							String strAmmo(SendMessage(ammoCombo, CB_GETLBTEXTLEN, curSel, 0), TEXT('\0'));
							SendMessage(ammoCombo, CB_GETLBTEXT, curSel, (LPARAM)&strAmmo.front());
							newData.weaponAmmo(ids[curSel]);
						}
					}
					catch (const std::out_of_range&)
					{
						ErrorBox(hDlg, TEXT("Invalid ammo type"));
						break;
					}

					Features::SetWeaponDataPtr(data, newData);
					Features::SetFirePowerTableEntry(data->statIndex(), newFirepower);
					Features::SetFiringSpeedTableEntry(data->statIndex(), newFiringSpeed);
				}
				[[fallthrough]];
				case IDCANCEL:
					data = nullptr;
					ids.clear();
					EndDialog(hDlg, 0);
					break;
			}
			break;
	}
	return FALSE;
}

BOOL CALLBACK ConfigDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static Bimap<UINT, String> keys;
	static KeyBindingsConfig bindings(TEXT(".\\bindings.ini"));

	switch (message)
	{
		case WM_INITDIALOG:
			if (keys.empty())
			{
				for (unsigned i = VK_LBUTTON; i <= VK_OEM_CLEAR; ++i)
				{
					String keyName(50, TEXT('\0'));
					if (auto scanCode = MapVirtualKey(i, MAPVK_VK_TO_VSC)) //If there's a mapping for this key
					{
						keyName.resize(GetKeyNameText(scanCode << 16, &keyName.front(), keyName.size()));
						if (!keyName.empty())
							keys.insert({ i, keyName });
					}
				}
			}

			for (const auto &key : keys)
				for (size_t i = 0, ctrlCnt = bindings.getControlCount(); i < ctrlCnt; ++i)
					SendMessage(GetDlgItem(hDlg, IDC_NOCLIP_KEY + i), CB_ADDSTRING, 0, (LPARAM)key.second.c_str());

			for (size_t i = 0, ctrlCnt = bindings.getControlCount(); i < ctrlCnt; ++i)
				SendMessage(GetDlgItem(hDlg, IDC_NOCLIP_KEY + i), CB_SETCURSEL, std::distance(keys.begin(), keys.find(bindings.getBinding(i))), 0);

			break;

		case WM_COMMAND:
			if (LOWORD(wParam) >= IDC_NOCLIP_KEY && LOWORD(wParam) < IDC_NOCLIP_KEY + bindings.getControlCount() && HIWORD(wParam) == CBN_SELCHANGE)
			{
				auto curSel = SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);
				String keyName(SendMessage((HWND)lParam, CB_GETLBTEXTLEN, curSel, 0), TEXT('\0'));

				SendMessage((HWND)lParam, CB_GETLBTEXT, curSel, (LPARAM)&keyName.front());
				bindings.setBinding(LOWORD(wParam) - IDC_NOCLIP_KEY, keys.getKey(keyName));
			}

			switch (LOWORD(wParam))
			{
				case IDOK:
				case IDCANCEL:
					EndDialog(hDlg, 0);
					break;
			}
			break;
	}
	return FALSE;
}