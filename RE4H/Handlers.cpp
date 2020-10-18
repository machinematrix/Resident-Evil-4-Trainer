#include "Handlers.h"

HWND mainWindowHandle;

void loggerCallback(const char *msgCouldBeNull1, const char*msgCouldBeNull2, const char*msg, ...)
{
	#ifndef NDEBUG
	using std::string;

	static std::mutex mtx;
	std::string formatString;
	string::size_type i = 0;
	va_list argumentList;
	void *returnAddress = nullptr;

	__asm
	{
		push eax
		mov eax, [ebp + 4]
		mov[returnAddress], eax
		pop eax
	}

	if (msgCouldBeNull1)
	{
		formatString = msgCouldBeNull1;
		va_start(argumentList, msgCouldBeNull1);
	}
	else if (msgCouldBeNull2)
	{
		formatString = msgCouldBeNull2;
		va_start(argumentList, msgCouldBeNull2);
	}
	else
	{
		formatString = msg;
		va_start(argumentList, msg);
	}

	while ((i = formatString.find('%', i)) != string::npos)
	{
		switch (formatString[i + 1])
		{
		case 's': {//string
			const char *strArg = va_arg(argumentList, const char *);
			formatString.replace(i, 2, strArg);
			i += strlen(strArg);
			break;
		}

		case 'i': //int
		case 'd': {
			std::string strInt = std::to_string(va_arg(argumentList, int));
			formatString.replace(i, 2, strInt);
			i += strInt.size();
			break;
		}

		case 'u': { //unsigned
			std::string strUnsigned = std::to_string(va_arg(argumentList, unsigned));
			formatString.replace(i, 2, strUnsigned);
			i += strUnsigned.size();
			break;
		}

		case 'f': { //float
			std::string strFloat = std::to_string(va_arg(argumentList, float));
			formatString.replace(i, 2, strFloat);
			i += strFloat.size();
			break;
		}

		default: {
			void *unknown = va_arg(argumentList, void *);
			++i;
			break;
		}
		}
	}

	va_end(argumentList);

	mtx.lock();
	std::cout << formatString << "\nReturn Address: " << returnAddress << '\n' << std::endl;
	mtx.unlock();

	#endif
}

LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
	if (!(lParam & 0x40000000))
		PostMessage(mainWindowHandle, WM_KEYDOWN, wParam, lParam);

	return CallNextHookEx(0, code, wParam, lParam);
}

void onWmCreate(HWND hWnd, WPARAM wParam, LPARAM lParam, MainWindowInfo &wndInfo)
{
	using namespace MenuIdentifiers;
	mainWindowHandle = hWnd;

	wndInfo.hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
	wndInfo.inventoryList.reset(new InventoryListView(0, 0, 0, 0, LVS_NOSORTHEADER | WS_VISIBLE | LVS_SINGLESEL | LVS_SHOWSELALWAYS, hWnd, (HMENU)InventoryList));
	wndInfo.addItem = CreateWindow(WC_BUTTON, TEXT("Add Item"), WS_VISIBLE | WS_CHILD | BS_CENTER, 0, 0, 0, 0, hWnd, (HMENU)AddItem, nullptr, nullptr);
	wndInfo.healthSet = CreateWindow(WC_BUTTON, TEXT("Set"), WS_VISIBLE | WS_CHILD | BS_CENTER, 0, 0, 0, 0, hWnd, (HMENU)HealthSet, nullptr, nullptr);
	wndInfo.healthGet = CreateWindow(WC_BUTTON, TEXT("Get"), WS_VISIBLE | WS_CHILD | BS_CENTER, 0, 0, 0, 0, hWnd, (HMENU)HealthGet, nullptr, nullptr);
	wndInfo.healthEdit = CreateWindow(WC_EDIT, nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER, 0, 0, 0, 0, hWnd, (HMENU)HealthEdit, nullptr, nullptr);

	wndInfo.healthLimitSet = CreateWindow(WC_BUTTON, TEXT("Set"), WS_VISIBLE | WS_CHILD | BS_CENTER, 0, 0, 0, 0, hWnd, (HMENU)HealthLimitSet, nullptr, nullptr);
	wndInfo.healthLimitGet = CreateWindow(WC_BUTTON, TEXT("Get"), WS_VISIBLE | WS_CHILD | BS_CENTER, 0, 0, 0, 0, hWnd, (HMENU)HealthLimitGet, nullptr, nullptr);
	wndInfo.healthLimitEdit = CreateWindow(WC_EDIT, nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER, 0, 0, 0, 0, hWnd, (HMENU)HealthLimitEdit, nullptr, nullptr);
	wndInfo.ashleyCheckbox = CreateWindow(WC_BUTTON, TEXT("Ashley"), WS_VISIBLE | WS_CHILD | BS_LEFT | BS_AUTOCHECKBOX, 0, 0, 0, 0, hWnd, (HMENU)ToggleAshley, nullptr, nullptr);
	wndInfo.tmpCheckbox = CreateWindow(WC_BUTTON, TEXT("Fast TMP"), WS_VISIBLE | WS_CHILD | BS_LEFT | BS_AUTOCHECKBOX, 0, 0, 0, 0, hWnd, (HMENU)ToggleTMPFireRate, nullptr, nullptr);
	wndInfo.noclipCheckbox = CreateWindow(WC_BUTTON, TEXT("Noclip"), WS_VISIBLE | WS_CHILD | BS_LEFT | BS_AUTOCHECKBOX, 0, 0, 0, 0, hWnd, (HMENU)ToggleNoclip, nullptr, nullptr);
	wndInfo.characterComboBox = CreateWindow(WC_COMBOBOX, nullptr, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)CharacterComboBox, nullptr, nullptr);
	wndInfo.costumeComboBox = CreateWindow(WC_COMBOBOX, nullptr, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)CostumeComboBox, nullptr, nullptr);
	wndInfo.difficultyComboBox = CreateWindow(WC_COMBOBOX, nullptr, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)DifficultyComboBox, nullptr, nullptr);
	wndInfo.sceneEdit = CreateWindow(WC_EDIT, nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER, 0, 0, 0, 0, hWnd, (HMENU)SceneEdit, nullptr, nullptr);
	wndInfo.sceneSet = CreateWindow(WC_BUTTON, TEXT("Set"), WS_VISIBLE | WS_CHILD | BS_CENTER, 0, 0, 0, 0, hWnd, (HMENU)SceneSet, nullptr, nullptr);
	wndInfo.healthText = CreateWindow(WC_STATIC, TEXT("Health:"), WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE, 0, 0, 0, 0, hWnd, (HMENU)HealthText, nullptr, nullptr);
	wndInfo.healthLimitText = CreateWindow(WC_STATIC, TEXT("Health Limit:"), WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE, 0, 0, 0, 0, hWnd, (HMENU)HealthLimitText, nullptr, nullptr);
	wndInfo.sceneText = CreateWindow(WC_STATIC, TEXT("Scene:"), WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE, 0, 0, 0, 0, hWnd, (HMENU)SceneText, nullptr, nullptr);
	wndInfo.doorText = CreateWindow(WC_STATIC, TEXT("Door:"), WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE, 0, 0, 0, 0, hWnd, (HMENU)DoorText, nullptr, nullptr);
	wndInfo.doorCombo = CreateWindow(WC_COMBOBOX, nullptr, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VISIBLE | WS_CHILD | WS_VSCROLL, 0, 0, 0, 0, hWnd, (HMENU)DoorCombo, nullptr, nullptr);
	wndInfo.doorButton = CreateWindow(WC_BUTTON, TEXT("Set"), WS_VISIBLE | WS_CHILD | BS_CENTER, 0, 0, 0, 0, hWnd, (HMENU)DoorButton, nullptr, nullptr);
	wndInfo.characterText = CreateWindow(WC_STATIC, TEXT("Character:"), WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE, 0, 0, 0, 0, hWnd, (HMENU)CharacterText, nullptr, nullptr);
	wndInfo.costumeText = CreateWindow(WC_STATIC, TEXT("Costume:"), WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE, 0, 0, 0, 0, hWnd, (HMENU)CostumeText, nullptr, nullptr);
	wndInfo.difficultyText = CreateWindow(WC_STATIC, TEXT("Difficulty:"), WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE, 0, 0, 0, 0, hWnd, (HMENU)DifficultyText, nullptr, nullptr);
	wndInfo.maxAmountEditButton = CreateWindow(WC_BUTTON, TEXT("Edit Stack Limit"), WS_VISIBLE | WS_CHILD | BS_CENTER, 0, 0, 0, 0, hWnd, (HMENU)MaxAmountEditButton, nullptr, nullptr);
	wndInfo.saveButton = CreateWindow(WC_BUTTON, TEXT("Save"), WS_VISIBLE | WS_CHILD | BS_CENTER, 0, 0, 0, 0, hWnd, (HMENU)SaveButton, nullptr, nullptr);
	wndInfo.merchantButton = CreateWindow(WC_BUTTON, TEXT("Merchant"), WS_VISIBLE | WS_CHILD | BS_CENTER, 0, 0, 0, 0, hWnd, (HMENU)MerchantButton, nullptr, nullptr);

	wndInfo.inventoryList->setExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_AUTOSIZECOLUMNS);
	wndInfo.inventoryList->addColumn(TEXT("Address"), 75);
	wndInfo.inventoryList->addColumn(TEXT("ID"), 50);
	wndInfo.inventoryList->addColumn(TEXT("Name"), 200);
	wndInfo.inventoryList->addColumn(TEXT("Amount"), 50);
	wndInfo.inventoryList->addColumn(TEXT("Inventory"), 75);
	wndInfo.inventoryList->addColumn(TEXT("FP"), 50);
	wndInfo.inventoryList->addColumn(TEXT("FS"), 50);
	wndInfo.inventoryList->addColumn(TEXT("RS"), 50);
	wndInfo.inventoryList->addColumn(TEXT("CA"), 50);
	SendMessage(wndInfo.healthEdit, EM_SETCUEBANNER, FALSE, (LPARAM)TEXT("Health"));
	SendMessage(wndInfo.healthLimitEdit, EM_SETCUEBANNER, FALSE, (LPARAM)TEXT("Health Limit"));
	SendMessage(wndInfo.sceneEdit, EM_SETCUEBANNER, FALSE, (LPARAM)TEXT("Scene"));
	SendMessage(wndInfo.inventoryList->getWindowHandle(), LVM_SETTEXTBKCOLOR, 0, CLR_HILIGHT);
	SendMessage(wndInfo.characterComboBox, CB_ADDSTRING, 0, (LPARAM)TEXT("Leon"));
	SendMessage(wndInfo.characterComboBox, CB_ADDSTRING, 0, (LPARAM)TEXT("Ashley"));
	SendMessage(wndInfo.characterComboBox, CB_ADDSTRING, 0, (LPARAM)TEXT("Ada"));
	SendMessage(wndInfo.characterComboBox, CB_ADDSTRING, 0, (LPARAM)TEXT("HUNK"));
	SendMessage(wndInfo.characterComboBox, CB_ADDSTRING, 0, (LPARAM)TEXT("Krauser"));
	SendMessage(wndInfo.characterComboBox, CB_ADDSTRING, 0, (LPARAM)TEXT("Wesker"));
	SendMessage(wndInfo.difficultyComboBox, CB_ADDSTRING, 0, (LPARAM)TEXT("Amateur"));
	SendMessage(wndInfo.difficultyComboBox, CB_ADDSTRING, 0, (LPARAM)TEXT("Easy"));
	SendMessage(wndInfo.difficultyComboBox, CB_ADDSTRING, 0, (LPARAM)TEXT("Normal"));
	SendMessage(wndInfo.difficultyComboBox, CB_ADDSTRING, 0, (LPARAM)TEXT("Professional"));

	wndInfo.kbHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, nullptr, GetWindowThreadProcessId(FindWindow(nullptr, TEXT("Resident Evil 4")), nullptr));
	//std::cout << GetLastError() << std::endl;
	EnumChildWindows(hWnd, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));
	SetTimer(hWnd, 1, 1500, nullptr);

	if (!wndInfo.cheats.good())
	{
		ErrorBox(hWnd, TEXT("Could not find one or more pointers"));
		DestroyWindow(hWnd);
		return;
	}

	wndInfo.cheats.setLoggerCallback(loggerCallback);
}

void onWmSize(HWND hWnd, WPARAM wParam, LPARAM lParam, MainWindowInfo &wndInfo)
{
	static int buttonWidth = LOWORD(GetDialogBaseUnits()) * 15, buttonHeight = static_cast<int>(7 * HIWORD(GetDialogBaseUnits()) / 4);
	const int width = LOWORD(lParam), height = HIWORD(lParam), spacing = 5;
	RECT lastWnd;
	constexpr BOOL update = TRUE;

	wndInfo.inventoryList->move(0, 0);
	wndInfo.inventoryList->resize(width, height / 3);

	lastWnd = GetWindowRectInParent(wndInfo.inventoryList->getWindowHandle());
	MoveWindow(wndInfo.addItem, lastWnd.left + spacing, lastWnd.bottom + spacing, buttonWidth, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.addItem);
	MoveWindow(wndInfo.maxAmountEditButton, lastWnd.right + spacing, lastWnd.top, buttonWidth, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.maxAmountEditButton);
	MoveWindow(wndInfo.saveButton, lastWnd.right + spacing, lastWnd.top, buttonWidth, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.saveButton);
	MoveWindow(wndInfo.merchantButton, lastWnd.right + spacing, lastWnd.top, buttonWidth, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.addItem);
	MoveWindow(wndInfo.healthText, lastWnd.left, lastWnd.bottom + spacing * 5, buttonWidth / 2, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.healthText);
	MoveWindow(wndInfo.healthEdit, lastWnd.right + spacing, lastWnd.top, buttonWidth / 2, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.healthEdit);
	MoveWindow(wndInfo.healthSet, lastWnd.right + spacing, lastWnd.top, buttonWidth / 3, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.healthSet);
	MoveWindow(wndInfo.healthGet, lastWnd.right + spacing, lastWnd.top, buttonWidth / 3, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.healthText);
	MoveWindow(wndInfo.healthLimitText, lastWnd.left, lastWnd.bottom + spacing, buttonWidth / 2, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.healthLimitText);
	MoveWindow(wndInfo.healthLimitEdit, lastWnd.right + spacing, lastWnd.top, buttonWidth / 2, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.healthLimitEdit);
	MoveWindow(wndInfo.healthLimitSet, lastWnd.right + spacing, lastWnd.top, buttonWidth / 3, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.healthLimitSet);
	MoveWindow(wndInfo.healthLimitGet, lastWnd.right + spacing, lastWnd.top, buttonWidth / 3, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.healthLimitText);
	MoveWindow(wndInfo.sceneText, lastWnd.left, lastWnd.bottom + spacing, buttonWidth / 2, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.sceneText);
	MoveWindow(wndInfo.sceneEdit, lastWnd.right + spacing, lastWnd.top, buttonWidth / 2, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.sceneEdit);
	MoveWindow(wndInfo.sceneSet, lastWnd.right + spacing, lastWnd.top, buttonWidth / 3, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.sceneText);
	MoveWindow(wndInfo.doorText, lastWnd.left, lastWnd.bottom + spacing, buttonWidth / 2, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.doorText);
	MoveWindow(wndInfo.doorCombo, lastWnd.right + spacing, lastWnd.top, buttonWidth, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.doorCombo);
	MoveWindow(wndInfo.doorButton, lastWnd.right + spacing, lastWnd.top, buttonWidth / 3, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.doorText);
	MoveWindow(wndInfo.noclipCheckbox, lastWnd.left, lastWnd.bottom + spacing, buttonWidth / 2, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.noclipCheckbox);
	MoveWindow(wndInfo.ashleyCheckbox, lastWnd.right + spacing, lastWnd.top, buttonWidth / 2, buttonHeight, update);
	MoveWindow(wndInfo.characterText, lastWnd.left, lastWnd.bottom + spacing, buttonWidth / 2, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.ashleyCheckbox);
	MoveWindow(wndInfo.tmpCheckbox, lastWnd.right + spacing, lastWnd.top, buttonWidth, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.characterText);
	MoveWindow(wndInfo.characterComboBox, lastWnd.right + spacing, lastWnd.top, buttonWidth, buttonHeight, update);
	MoveWindow(wndInfo.costumeText, lastWnd.left, lastWnd.bottom + spacing, buttonWidth / 2, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.costumeText);
	MoveWindow(wndInfo.costumeComboBox, lastWnd.right + spacing, lastWnd.top, buttonWidth, buttonHeight, update);
	MoveWindow(wndInfo.difficultyText, lastWnd.left, lastWnd.bottom + spacing, buttonWidth / 2, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.difficultyText);
	MoveWindow(wndInfo.difficultyComboBox, lastWnd.right + spacing, lastWnd.top, buttonWidth, buttonHeight, update);
	if (!update) UpdateWindow(hWnd);
}

void onWmTimer(HWND hWnd, WPARAM wParam, LPARAM lParam, MainWindowInfo &wndInfo)
{
	if (wndInfo.cheats.good())
	{
		HWND focused = GetFocus();
		//health
		auto strHealth = GetControlText(wndInfo.healthEdit);
		auto health = wndInfo.cheats.getHealth();

		//health limit
		auto strHealthLimit = GetControlText(wndInfo.healthLimitEdit);
		auto healthLimit = wndInfo.cheats.getHealthLimit();

		//scene
		auto strScene = GetControlText(wndInfo.sceneEdit);
		auto scene = wndInfo.cheats.getScene();

		//character
		std::int8_t cbChar = static_cast<std::uint8_t>(SendMessage(wndInfo.characterComboBox, CB_GETCURSEL, 0, 0)), currChar = wndInfo.cheats.getCharacter();
		std::int8_t cbCostume = static_cast<std::uint8_t>(SendMessage(wndInfo.costumeComboBox, CB_GETCURSEL, 0, 0)), currCostume = wndInfo.cheats.getCostume();

		//difficulty
		Game::Difficulty currDiff = wndInfo.cheats.getDifficulty();
		auto cbDifficulty = SendMessage(wndInfo.difficultyComboBox, CB_GETCURSEL, 0, 0);

		if (cbChar != currChar) {
			SendMessage(wndInfo.characterComboBox, CB_SETCURSEL, currChar, 0);
			SendMessage(wndInfo.costumeComboBox, CB_RESETCONTENT, 0, 0);
			for (const auto &costumeName : wndInfo.cheats.getCharacterCostumeNames(currChar))
				SendMessage(wndInfo.costumeComboBox, CB_ADDSTRING, 0, (LPARAM)costumeName.c_str());
			wndInfo.cheats.setCostume(0);
			currCostume = wndInfo.cheats.getCostume();
		}

		if (cbCostume != currCostume || cbChar != currChar) //If the character or the costume changed
		{
			if (currChar == 2 && currCostume == 3) {
				if (cbCostume != 2)
					SendMessage(wndInfo.costumeComboBox, CB_SETCURSEL, 2, 0);
			}
			else
				SendMessage(wndInfo.costumeComboBox, CB_SETCURSEL, currCostume, 0);
		}

		switch (currDiff) //update difficulty combo box
		{
		case Game::Difficulty::AMATEUR:
			if (cbDifficulty != 0)
				SendMessage(wndInfo.difficultyComboBox, CB_SETCURSEL, 0, 0);
			break;
		case Game::Difficulty::EASY:
			if (cbDifficulty != 1)
				SendMessage(wndInfo.difficultyComboBox, CB_SETCURSEL, 1, 0);
			break;
		case Game::Difficulty::NORMAL:
			if (cbDifficulty != 2)
				SendMessage(wndInfo.difficultyComboBox, CB_SETCURSEL, 2, 0);
			break;
		case Game::Difficulty::PROFESSIONAL:
			if (cbDifficulty != 3)
				SendMessage(wndInfo.difficultyComboBox, CB_SETCURSEL, 3, 0);
			break;
		}

		if (focused != wndInfo.sceneEdit)
		{
			try {
				if (strScene.empty() || scene != std::stoul(strScene, nullptr, 16))
					SetWindowText(wndInfo.sceneEdit, toHexWstring(scene).c_str());
			}
			catch (const std::invalid_argument&)
			{}
		}

		if (wndInfo.cheats.doorListChanged())
		{
			std::uint32_t i = 0;
			SendMessage(wndInfo.doorCombo, CB_RESETCONTENT, 0, 0);
			for (const auto door : wndInfo.cheats.getDoors()) {
				SendMessage(wndInfo.doorCombo, CB_ADDSTRING, 0, (LPARAM)std::to_wstring(i).c_str());
				++i;
			}
		}

		if (focused != wndInfo.healthEdit)
		{
			try {
				if (strHealth.empty() || health != std::stoul(strHealth))
					SetWindowText(wndInfo.healthEdit, std::to_wstring(health).c_str());
			}
			catch (const std::invalid_argument &) {}
		}

		if (focused != wndInfo.healthLimitEdit)
		{
			try {
				if (strHealthLimit.empty() || healthLimit != std::stoul(strHealthLimit))
					SetWindowText(wndInfo.healthLimitEdit, std::to_wstring(healthLimit).c_str());
			}
			catch (const std::invalid_argument&)
			{}
		}

		wndInfo.inventoryList->refresh(wndInfo.cheats);
		SendMessage(wndInfo.ashleyCheckbox, BM_SETCHECK, (WPARAM)wndInfo.cheats.isAshleyPresent(), 0);
		SendMessage(wndInfo.noclipCheckbox, BM_SETCHECK, (WPARAM)wndInfo.cheats.isNoclipOn(), 0);
		SendMessage(wndInfo.tmpCheckbox, BM_SETCHECK, (WPARAM)wndInfo.cheats.isFastTmpEnabled(), 0);
	}
}

void onWmCommand(HWND hWnd, WPARAM wParam, LPARAM lParam, MainWindowInfo &wndInfo)
{
	using namespace MenuIdentifiers;
	switch (LOWORD(wParam))
	{
	case HealthLimitSet:
	case HealthSet: {
		if (wndInfo.cheats.good())
		{
			String text = GetControlText(LOWORD(wParam) == HealthSet ? wndInfo.healthEdit : wndInfo.healthLimitEdit);
			void (Game:: * setter)(std::uint16_t) = (LOWORD(wParam) == HealthSet ? &Game::setHealth : &Game::setHealthLimit);

			try { (wndInfo.cheats.*setter)(std::stoi(text)); }
			catch (const std::invalid_argument &) {
				ErrorBox(hWnd, TEXT("Invalid value"));
				break;
			}
		}
		else ErrorBox(hWnd, TEXT("Hacks not initialized"));
		break;
	}
	case HealthLimitGet:
	case HealthGet: {
		if (wndInfo.cheats.good()) {
			auto getter = (LOWORD(wParam) == HealthGet ? &Game::getHealth : &Game::getHealthLimit);
			std::wstring health = std::to_wstring((wndInfo.cheats.*getter)());
			SetWindowText(LOWORD(wParam) == HealthGet ? wndInfo.healthEdit : wndInfo.healthLimitEdit, health.c_str());
		}
		else ErrorBox(hWnd, TEXT("Hacks not initialized"));
		break;
	}
	case EditItem: {
		auto selectedItem = wndInfo.inventoryList->getSelectedItem();
		if (selectedItem != -1)
		{
			if (auto itemPtr = wndInfo.inventoryList->getItemAddress(selectedItem))
			{
				DialogInfo info(&wndInfo.cheats, itemPtr);
				DialogBoxParam(wndInfo.hInstance, MAKEINTRESOURCE(ItemDialog), hWnd, ItemDlgProc, (LPARAM)& info);
				if (DWORD error = GetLastError()) {
					String msg = TEXT("Could not create dialog: ");
					msg += std::to_wstring(error);
					ErrorBox(hWnd, msg.c_str());
				}
			}
			else ErrorBox(hWnd, TEXT("Null item pointer"));
		}
		else ErrorBox(hWnd, TEXT("Select an item first"));
		break;
	}
	case AddItem: {
		if (auto itemPtr = wndInfo.cheats.addItem())
		{
			itemPtr->amount(1);
			itemPtr->valid(0);
			itemPtr->inInventory(0);
			itemPtr->posX(8);
			itemPtr->posY(15);
			itemPtr->rotation(1);
			DialogInfo info(&wndInfo.cheats, itemPtr);
			auto result = DialogBoxParam(wndInfo.hInstance, MAKEINTRESOURCE(ItemDialog), hWnd, ItemDlgProc, (LPARAM)& info);
			if (result == -1) {
				String msg = TEXT("Could not create dialog: ");
				msg += std::to_wstring(GetLastError());
				ErrorBox(hWnd, msg.c_str());
			}
			else itemPtr->valid(static_cast<std::uint8_t>(result));
		}
		else {
			ErrorBox(hWnd, TEXT("Inventory is full"));
		}
		break;
	}
	case MaxAmountEditButton: {
		DialogBoxParam(wndInfo.hInstance, MAKEINTRESOURCE(ItemAmountDialog), hWnd, EditMaxAmountDlgProc, (LPARAM)& wndInfo.cheats);
		break;
	}
	case EraseItem: {
		wndInfo.inventoryList->eraseSelectedItem();
		break;
	}
	case ToggleAshley: {
		wndInfo.cheats.toggleAshley(SendMessage(wndInfo.ashleyCheckbox, BM_GETCHECK, 0, 0) ? true : false);
		break;
	}
	case ToggleTMPFireRate: {
		wndInfo.cheats.toggleFastTmp(SendMessage(wndInfo.tmpCheckbox, BM_GETCHECK, 0, 0) ? true : false);
		break;
	}
	case ToggleNoclip: {
		wndInfo.cheats.toggleNoclip(SendMessage(wndInfo.noclipCheckbox, BM_GETCHECK, 0, 0) ? true : false);
		break;
	}
	case CharacterComboBox: {
		if (HIWORD(wParam) == CBN_SELCHANGE)
		{
			std::uint8_t character = static_cast<std::uint8_t>(SendMessage(wndInfo.characterComboBox, CB_GETCURSEL, 0, 0));
			if (character != CB_ERR)
			{
				wndInfo.cheats.setCharacter(character);
				SendMessage(wndInfo.costumeComboBox, CB_RESETCONTENT, 0, 0);

				for (const auto &costumeName : wndInfo.cheats.getCharacterCostumeNames(wndInfo.cheats.getCharacter())) {
					SendMessage(wndInfo.costumeComboBox, CB_ADDSTRING, 0, (LPARAM)costumeName.c_str());
				}
			}
			else ErrorBox(hWnd, TEXT("Invalid character"));
		}
		break;
	}
	case CostumeComboBox: {
		if (HIWORD(wParam) == CBN_SELCHANGE) {
			std::uint8_t cbCostume = static_cast<std::uint8_t>(SendMessage(wndInfo.costumeComboBox, CB_GETCURSEL, 0, 0));
			if (wndInfo.cheats.getCharacter() == 2 && cbCostume == 2) wndInfo.cheats.setCostume(3); //remove this from front end
			else {
				if (cbCostume != CB_ERR) wndInfo.cheats.setCostume(cbCostume);
				else ErrorBox(hWnd, TEXT("Invalid costume"));
			}
		}
		break;
	}
	case DifficultyComboBox: {
		if (HIWORD(wParam) == CBN_SELCHANGE) {
			switch (SendMessage(wndInfo.difficultyComboBox, CB_GETCURSEL, 0, 0))
			{
			case 0:
				wndInfo.cheats.setDifficulty(Game::Difficulty::AMATEUR);
				break;
			case 1:
				wndInfo.cheats.setDifficulty(Game::Difficulty::EASY);
				break;
			case 2:
				wndInfo.cheats.setDifficulty(Game::Difficulty::NORMAL);
				break;
			case 3:
				wndInfo.cheats.setDifficulty(Game::Difficulty::PROFESSIONAL);
				break;
			default:
				ErrorBox(hWnd, TEXT("Select a difficulty"));
				break;
			}
		}
		break;
	}
	case EditItemStats: {
		auto selectedItem = wndInfo.inventoryList->getSelectedItem();
		if (selectedItem != -1)
		{
			WeaponStatsInfo info(&wndInfo.cheats, wndInfo.cheats.getWeaponDataPtr(wndInfo.inventoryList->getItemAddress(selectedItem)->itemId()));
			DialogBoxParam(wndInfo.hInstance, MAKEINTRESOURCE(DLG_WEAPONS_STATS_DIALOG), hWnd, WeaponDlgProc, (LPARAM)& info);
			if (DWORD error = GetLastError()) {
				String msg = TEXT("Could not create dialog: ");
				msg += std::to_wstring(error);
				ErrorBox(hWnd, msg.c_str());
			}
		}
		else ErrorBox(hWnd, TEXT("Select an item first"));
		break;
	}
	case SceneSet: {
		try {
			wndInfo.cheats.setScene(std::stoul(GetControlText(wndInfo.sceneEdit), nullptr, 16));
			//wndInfo.hacks.spawnPickup(41, 0);
		}
		catch (const std::invalid_argument &) {
			ErrorBox(hWnd, TEXT("Invalid scene"));
		}
		break;
	}
	case DoorButton: {
		auto curSel = SendMessage(wndInfo.doorCombo, CB_GETCURSEL, 0, 0);
		if (curSel != CB_ERR)
		{
			if (static_cast<size_t>(curSel) < wndInfo.cheats.getDoors().size())
				wndInfo.cheats.useDoor(wndInfo.cheats.getDoors()[curSel]);
			else
				ErrorBox(hWnd, TEXT("Invalid door"));
		}
		else
			ErrorBox(hWnd, TEXT("Select a door first"));
		break;
	}
	case SaveButton: {
		wndInfo.cheats.openTypewriter(Game::TypewriterMode::SAVE);
		break;
	}
	case MerchantButton: {
		wndInfo.cheats.openMerchant();
		break;
	}
	case KeyBindings: {
		DialogBoxParam(wndInfo.hInstance, MAKEINTRESOURCE(DLG_KEYBINDINGS), hWnd, ConfigDlgProc, NULL);
		break;
	}
	case Exit:
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	}
}

void onWmNotify(HWND hWnd, WPARAM wParam, LPARAM lParam, MainWindowInfo &wndInfo)
{
	const NMHDR &header = *(NMHDR *)lParam;

	switch (header.idFrom)
	{
	case MenuIdentifiers::InventoryList: {
		if (header.code == NM_RCLICK)
		{
			const NMITEMACTIVATE &item = *(NMITEMACTIVATE *)lParam;
			if (item.iItem >= 0 && item.iItem < static_cast<int>(wndInfo.inventoryList->itemCount()))
			{
				if (HMENU hMenu = CreatePopupMenu())
				{
					POINT cursorPos = item.ptAction;
					ClientToScreen(hWnd, &cursorPos);
					AppendMenu(hMenu, MF_ENABLED | MF_STRING, MenuIdentifiers::EditItem, TEXT("Edit Item"));
					AppendMenu(hMenu, (wndInfo.cheats.isWeapon(wndInfo.inventoryList->getItemAddress(item.iItem)->itemId()) ? MF_ENABLED : MF_GRAYED) | MF_STRING, MenuIdentifiers::EditItemStats, TEXT("Edit Item Stats"));
					AppendMenu(hMenu, MF_ENABLED | MF_STRING, MenuIdentifiers::EraseItem, TEXT("Erase Item"));
					if (BOOL result = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_TOPALIGN | TPM_LEFTALIGN | TPM_LEFTBUTTON, cursorPos.x, cursorPos.y, 0, item.hdr.hwndFrom, nullptr)) {
						DestroyMenu(hMenu);
						SendMessage(hWnd, WM_COMMAND, MAKEWORD(result, 0), 0);
					}
					else DestroyMenu(hMenu);
				}
				else ErrorBox(hWnd, TEXT("Could not create context menu"));
			}
		}
		break;
	}
	}
}

void onWmKeydown(HWND hWnd, WPARAM wParam, LPARAM lParam, MainWindowInfo &wndInfo)
{
	static const KeyBindingsConfig bindings(TEXT(".\\bindings.ini"));

	for (size_t i = 0, ctrlCnt = bindings.getControlCount(); i < ctrlCnt; ++i)
	{
		if (bindings.getBinding(i) == wParam)
		{
			bool toggle;
			switch (i)
			{
			case KeyBindingsConfig::NOCLIP:
				toggle = !wndInfo.cheats.isNoclipOn();
				wndInfo.cheats.toggleNoclip(toggle);
				SendMessage(wndInfo.noclipCheckbox, BM_SETCHECK, toggle, 0);
				break;
			case KeyBindingsConfig::ASHLEY:
				toggle = !wndInfo.cheats.isAshleyPresent();
				wndInfo.cheats.toggleAshley(toggle);
				SendMessage(wndInfo.ashleyCheckbox, BM_SETCHECK, toggle, 0);
				break;
			case KeyBindingsConfig::HEAL:
				wndInfo.cheats.setHealth(wndInfo.cheats.getHealthLimit());
				break;
			case KeyBindingsConfig::SAVE:
				wndInfo.cheats.openTypewriter(Game::TypewriterMode::SAVE);
				break;
			case KeyBindingsConfig::MERCHANT:
				wndInfo.cheats.openMerchant();
				break;
			case KeyBindingsConfig::MELEE1:
				wndInfo.cheats.melee(Game::MeleeType::HEAD);
				break;
			case KeyBindingsConfig::MELEE2:
				wndInfo.cheats.melee(Game::MeleeType::KNEE);
				break;
			case KeyBindingsConfig::INCREASE_ALTITUDE:
			{
				if (auto coordinates = wndInfo.cheats.getPlayerCoordinates())
				{
					coordinates.value()[1] += 1000.f;
					wndInfo.cheats.setPlayerCoordinates(coordinates.value());
				}
				break;
			}
			case KeyBindingsConfig::DECREASE_ALTITUDE:
			{
				if (auto coordinates = wndInfo.cheats.getPlayerCoordinates())
				{
					coordinates.value()[1] -= 1000.f;
					wndInfo.cheats.setPlayerCoordinates(coordinates.value());
				}
				break;
			}
			}
		}
	}
}

void onWmDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam, MainWindowInfo &wndInfo)
{
	KillTimer(hWnd, 1); //kill timer BEFORE destroying the list, else we'll dereference a nullptr!
	wndInfo.inventoryList.reset();
	UnhookWindowsHookEx(wndInfo.kbHook);
	PostQuitMessage(0);
}