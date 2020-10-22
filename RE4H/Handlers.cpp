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
	wndInfo.addItem = CreateWindow(WC_BUTTON, TEXT("Add Item"), WS_VISIBLE | WS_CHILD | BS_CENTER, 0, 0, 0, 0, hWnd, (HMENU)ADD_ITEM, nullptr, nullptr);
	wndInfo.healthSet = CreateWindow(WC_BUTTON, TEXT("Set"), WS_VISIBLE | WS_CHILD | BS_CENTER, 0, 0, 0, 0, hWnd, (HMENU)HEALTH_SET, nullptr, nullptr);
	wndInfo.healthGet = CreateWindow(WC_BUTTON, TEXT("Get"), WS_VISIBLE | WS_CHILD | BS_CENTER, 0, 0, 0, 0, hWnd, (HMENU)HEALTH_GET, nullptr, nullptr);
	wndInfo.healthEdit = CreateWindow(WC_EDIT, nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER, 0, 0, 0, 0, hWnd, (HMENU)HealthEdit, nullptr, nullptr);

	wndInfo.healthLimitSet = CreateWindow(WC_BUTTON, TEXT("Set"), WS_VISIBLE | WS_CHILD | BS_CENTER, 0, 0, 0, 0, hWnd, (HMENU)HEALTH_LIMIT_SET, nullptr, nullptr);
	wndInfo.healthLimitGet = CreateWindow(WC_BUTTON, TEXT("Get"), WS_VISIBLE | WS_CHILD | BS_CENTER, 0, 0, 0, 0, hWnd, (HMENU)HEALTH_LIMIT_GET, nullptr, nullptr);
	wndInfo.healthLimitEdit = CreateWindow(WC_EDIT, nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER, 0, 0, 0, 0, hWnd, (HMENU)HealthLimitEdit, nullptr, nullptr);
	wndInfo.noclipCheckbox = CreateWindow(WC_BUTTON, TEXT("Noclip"), WS_VISIBLE | WS_CHILD | BS_LEFT | BS_AUTOCHECKBOX, 0, 0, 0, 0, hWnd, (HMENU)TOGGLE_NOCLIP, nullptr, nullptr);
	wndInfo.ashleyCheckbox = CreateWindow(WC_BUTTON, TEXT("Ashley"), WS_VISIBLE | WS_CHILD | BS_LEFT | BS_AUTOCHECKBOX, 0, 0, 0, 0, hWnd, (HMENU)TOGGLE_ASHLEY, nullptr, nullptr);
	wndInfo.tmpCheckbox = CreateWindow(WC_BUTTON, TEXT("Fast TMP"), WS_VISIBLE | WS_CHILD | BS_LEFT | BS_AUTOCHECKBOX, 0, 0, 0, 0, hWnd, (HMENU)TOGGLE_TMP_FIRE_RATE, nullptr, nullptr);
	wndInfo.easyDropsCheckbox = CreateWindow(WC_BUTTON, TEXT("Easy Drops"), WS_VISIBLE | WS_CHILD | BS_LEFT | BS_AUTOCHECKBOX, 0, 0, 0, 0, hWnd, (HMENU)TOGGLE_EASY_DROPS, nullptr, nullptr);
	wndInfo.characterComboBox = CreateWindow(WC_COMBOBOX, nullptr, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)CHARACTER_COMBO_BOX, nullptr, nullptr);
	wndInfo.costumeComboBox = CreateWindow(WC_COMBOBOX, nullptr, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)COSTUME_COMBO_BOX, nullptr, nullptr);
	wndInfo.difficultyComboBox = CreateWindow(WC_COMBOBOX, nullptr, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)DIFFICULTY_COMBO_BOX, nullptr, nullptr);
	wndInfo.sceneCombo = CreateWindow(WC_COMBOBOX, nullptr, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VISIBLE | WS_VSCROLL | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)SceneCombo, nullptr, nullptr);
	wndInfo.sceneSet = CreateWindow(WC_BUTTON, TEXT("Set"), WS_VISIBLE | WS_CHILD | BS_CENTER, 0, 0, 0, 0, hWnd, (HMENU)SCENE_SET, nullptr, nullptr);
	wndInfo.healthText = CreateWindow(WC_STATIC, TEXT("Health:"), WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE, 0, 0, 0, 0, hWnd, (HMENU)HealthText, nullptr, nullptr);
	wndInfo.healthLimitText = CreateWindow(WC_STATIC, TEXT("Health Limit:"), WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE, 0, 0, 0, 0, hWnd, (HMENU)HealthLimitText, nullptr, nullptr);
	wndInfo.sceneText = CreateWindow(WC_STATIC, TEXT("Scene:"), WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE, 0, 0, 0, 0, hWnd, (HMENU)SceneText, nullptr, nullptr);
	wndInfo.doorText = CreateWindow(WC_STATIC, TEXT("Door:"), WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE, 0, 0, 0, 0, hWnd, (HMENU)DoorText, nullptr, nullptr);
	wndInfo.doorCombo = CreateWindow(WC_COMBOBOX, nullptr, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VISIBLE | WS_CHILD | WS_VSCROLL, 0, 0, 0, 0, hWnd, (HMENU)DoorCombo, nullptr, nullptr);
	wndInfo.doorButton = CreateWindow(WC_BUTTON, TEXT("Set"), WS_VISIBLE | WS_CHILD | BS_CENTER, 0, 0, 0, 0, hWnd, (HMENU)DOOR_BUTTON, nullptr, nullptr);
	wndInfo.characterText = CreateWindow(WC_STATIC, TEXT("Character:"), WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE, 0, 0, 0, 0, hWnd, (HMENU)CharacterText, nullptr, nullptr);
	wndInfo.costumeText = CreateWindow(WC_STATIC, TEXT("Costume:"), WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE, 0, 0, 0, 0, hWnd, (HMENU)CostumeText, nullptr, nullptr);
	wndInfo.difficultyText = CreateWindow(WC_STATIC, TEXT("Difficulty:"), WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE, 0, 0, 0, 0, hWnd, (HMENU)DifficultyText, nullptr, nullptr);
	wndInfo.maxAmountEditButton = CreateWindow(WC_BUTTON, TEXT("Edit Stack Limit"), WS_VISIBLE | WS_CHILD | BS_CENTER, 0, 0, 0, 0, hWnd, (HMENU)MAX_AMOUNT_EDIT_BUTTON, nullptr, nullptr);
	wndInfo.saveButton = CreateWindow(WC_BUTTON, TEXT("Save"), WS_VISIBLE | WS_CHILD | BS_CENTER, 0, 0, 0, 0, hWnd, (HMENU)SAVE_BUTTON, nullptr, nullptr);
	wndInfo.merchantButton = CreateWindow(WC_BUTTON, TEXT("Merchant"), WS_VISIBLE | WS_CHILD | BS_CENTER, 0, 0, 0, 0, hWnd, (HMENU)MERCHANT_BUTTON, nullptr, nullptr);

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
	SendMessage(wndInfo.sceneCombo, CB_SETMINVISIBLE, 20, 0);

	for (std::wstring_view scene : wndInfo.game.getSceneFileNames())
		SendMessage(wndInfo.sceneCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(scene.data()));

	wndInfo.kbHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, nullptr, GetWindowThreadProcessId(FindWindow(nullptr, TEXT("Resident Evil 4")), nullptr));

	EnumChildWindows(hWnd, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));
	SetTimer(hWnd, 1, 1500, nullptr);

	if (!wndInfo.game.good())
	{
		ErrorBox(hWnd, TEXT("Could not find one or more pointers"));
		DestroyWindow(hWnd);
		return;
	}

	auto callback = [&wndInfo](std::uint32_t newScene, const std::vector<void*> &doors)
	{
		std::wostringstream stream;
		std::uint32_t i = 0;
		int curSel;

		SendMessage(wndInfo.doorCombo, CB_RESETCONTENT, 0, 0);
		for (const auto door : doors)
		{
			SendMessage(wndInfo.doorCombo, CB_ADDSTRING, 0, (LPARAM)std::to_wstring(i).c_str());
			++i;
		}

		stream << TEXT('r') << std::hex << newScene << TEXT(".udas.lfs");
		curSel = ComboBox_FindStringExact(wndInfo.sceneCombo, -1, stream.str().c_str());

		if (curSel != CB_ERR)
			ComboBox_SetCurSel(wndInfo.sceneCombo, curSel);
		else
			SendMessage(wndInfo.sceneCombo, WM_CLEAR, 0, 0);
	};

	wndInfo.game.setLoggerCallback(loggerCallback);
	wndInfo.game.setDoorListUpdateCallback(callback);

	SendMessage(wndInfo.ashleyCheckbox, BM_SETCHECK, (WPARAM)wndInfo.game.isAshleyPresent(), 0);
	SendMessage(wndInfo.noclipCheckbox, BM_SETCHECK, (WPARAM)wndInfo.game.isNoclipOn(), 0);
	SendMessage(wndInfo.tmpCheckbox, BM_SETCHECK, (WPARAM)wndInfo.game.isFastTmpEnabled(), 0);
	SendMessage(wndInfo.easyDropsCheckbox, BM_SETCHECK, (WPARAM)wndInfo.game.easyDrops(), 0);
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
	MoveWindow(wndInfo.sceneCombo, lastWnd.right + spacing, lastWnd.top, buttonWidth, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.sceneCombo);
	MoveWindow(wndInfo.sceneSet, lastWnd.right + spacing, lastWnd.top, buttonWidth / 3, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.sceneText);
	MoveWindow(wndInfo.doorText, lastWnd.left, lastWnd.bottom + spacing, buttonWidth / 2, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.doorText);
	MoveWindow(wndInfo.doorCombo, lastWnd.right + spacing, lastWnd.top, buttonWidth, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.doorCombo);
	MoveWindow(wndInfo.doorButton, lastWnd.right + spacing, lastWnd.top, buttonWidth / 3, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.doorText);
	MoveWindow(wndInfo.noclipCheckbox, lastWnd.left, lastWnd.bottom + spacing, buttonWidth * 60 / 100, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.noclipCheckbox);
	MoveWindow(wndInfo.ashleyCheckbox, lastWnd.right + spacing, lastWnd.top, buttonWidth * 60 / 100, buttonHeight, update);
	MoveWindow(wndInfo.characterText, lastWnd.left, lastWnd.bottom + spacing, buttonWidth / 2, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.ashleyCheckbox);
	MoveWindow(wndInfo.tmpCheckbox, lastWnd.right + spacing, lastWnd.top, buttonWidth * 60 / 100, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.tmpCheckbox);
	MoveWindow(wndInfo.easyDropsCheckbox, lastWnd.right + spacing, lastWnd.top, buttonWidth * 60 / 100, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.characterText);
	MoveWindow(wndInfo.characterComboBox, lastWnd.right + spacing, lastWnd.top, buttonWidth, buttonHeight, update);
	MoveWindow(wndInfo.costumeText, lastWnd.left, lastWnd.bottom + spacing, buttonWidth / 2, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.costumeText);
	MoveWindow(wndInfo.costumeComboBox, lastWnd.right + spacing, lastWnd.top, buttonWidth, buttonHeight, update);
	MoveWindow(wndInfo.difficultyText, lastWnd.left, lastWnd.bottom + spacing, buttonWidth / 2, buttonHeight, update);

	lastWnd = GetWindowRectInParent(wndInfo.difficultyText);
	MoveWindow(wndInfo.difficultyComboBox, lastWnd.right + spacing, lastWnd.top, buttonWidth, buttonHeight, update);
	
	if (!update)
		UpdateWindow(hWnd);
}

void onWmTimer(HWND hWnd, WPARAM wParam, LPARAM lParam, MainWindowInfo &wndInfo)
{
	if (wndInfo.game.good())
	{
		HWND focused = GetFocus();
		//health
		auto strHealth = GetControlText(wndInfo.healthEdit);
		auto health = wndInfo.game.getHealth();

		//health limit
		auto strHealthLimit = GetControlText(wndInfo.healthLimitEdit);
		auto healthLimit = wndInfo.game.getHealthLimit();

		//scene
		auto scene = wndInfo.game.getScene();

		//character
		std::int8_t cbChar = static_cast<std::uint8_t>(SendMessage(wndInfo.characterComboBox, CB_GETCURSEL, 0, 0)), currChar = wndInfo.game.getCharacter();
		std::int8_t cbCostume = static_cast<std::uint8_t>(SendMessage(wndInfo.costumeComboBox, CB_GETCURSEL, 0, 0)), currCostume = wndInfo.game.getCostume();

		//difficulty
		Game::Difficulty currDiff = wndInfo.game.getDifficulty();
		auto cbDifficulty = SendMessage(wndInfo.difficultyComboBox, CB_GETCURSEL, 0, 0);

		if (cbChar != currChar) {
			SendMessage(wndInfo.characterComboBox, CB_SETCURSEL, currChar, 0);
			SendMessage(wndInfo.costumeComboBox, CB_RESETCONTENT, 0, 0);
			for (const auto &costumeName : wndInfo.game.getCharacterCostumeNames(currChar))
				SendMessage(wndInfo.costumeComboBox, CB_ADDSTRING, 0, (LPARAM)costumeName.c_str());
			wndInfo.game.setCostume(0);
			currCostume = wndInfo.game.getCostume();
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

		wndInfo.inventoryList->refresh(wndInfo.game);
		SendMessage(wndInfo.ashleyCheckbox, BM_SETCHECK, (WPARAM)wndInfo.game.isAshleyPresent(), 0);
		SendMessage(wndInfo.noclipCheckbox, BM_SETCHECK, (WPARAM)wndInfo.game.isNoclipOn(), 0);
		SendMessage(wndInfo.tmpCheckbox, BM_SETCHECK, (WPARAM)wndInfo.game.isFastTmpEnabled(), 0);
		SendMessage(wndInfo.easyDropsCheckbox, BM_SETCHECK, (WPARAM)wndInfo.game.easyDrops(), 0);
	}
}

void onWmCommand(HWND hWnd, WPARAM wParam, LPARAM lParam, MainWindowInfo &wndInfo)
{
	using namespace MenuIdentifiers;

	switch (LOWORD(wParam))
	{
		case HEALTH_LIMIT_SET:
		case HEALTH_SET:
		{
			String text = GetControlText(LOWORD(wParam) == HEALTH_SET ? wndInfo.healthEdit : wndInfo.healthLimitEdit);
			void (Game:: * setter)(std::uint16_t) = (LOWORD(wParam) == HEALTH_SET ? &Game::setHealth : &Game::setHealthLimit);

			try {
				(wndInfo.game.*setter)(std::stoi(text));
			}
			catch (const std::invalid_argument &) {
				ErrorBox(hWnd, TEXT("Invalid value"));
				break;
			}
			break;
		}
		case HEALTH_LIMIT_GET:
		case HEALTH_GET:
		{
			auto getter = (LOWORD(wParam) == HEALTH_GET ? &Game::getHealth : &Game::getHealthLimit);
			std::wstring health = std::to_wstring((wndInfo.game.*getter)());
			SetWindowText(LOWORD(wParam) == HEALTH_GET ? wndInfo.healthEdit : wndInfo.healthLimitEdit, health.c_str());
			break;
		}
		case EDIT_ITEM:
		{
			auto selectedItem = wndInfo.inventoryList->getSelectedItem();

			if (selectedItem != -1)
			{
				if (auto itemPtr = wndInfo.inventoryList->getItemAddress(selectedItem))
				{
					DialogInfo info(&wndInfo.game, itemPtr);
					DialogBoxParam(wndInfo.hInstance, MAKEINTRESOURCE(ItemDialog), hWnd, ItemDlgProc, (LPARAM)&info);

					if (DWORD error = GetLastError())
					{
						String msg = TEXT("Could not create dialog: ");
						msg += std::to_wstring(error);
						ErrorBox(hWnd, msg.c_str());
					}
				}
				else
					ErrorBox(hWnd, TEXT("Null item pointer"));
			}
			else
				ErrorBox(hWnd, TEXT("Select an item first"));
			break;
		}

		case ADD_ITEM:
			if (auto itemPtr = wndInfo.game.addItem())
			{
				itemPtr->amount(1);
				itemPtr->valid(0);
				itemPtr->inInventory(0);
				itemPtr->posX(8);
				itemPtr->posY(15);
				itemPtr->rotation(1);
				DialogInfo info(&wndInfo.game, itemPtr);
				auto result = DialogBoxParam(wndInfo.hInstance, MAKEINTRESOURCE(ItemDialog), hWnd, ItemDlgProc, (LPARAM)&info);
				if (result == -1)
				{
					String msg = TEXT("Could not create dialog: ");
					msg += std::to_wstring(GetLastError());
					ErrorBox(hWnd, msg.c_str());
				}
				else
					itemPtr->valid(static_cast<std::uint8_t>(result));
			}
			else
				ErrorBox(hWnd, TEXT("Inventory is full"));
			break;

		case MAX_AMOUNT_EDIT_BUTTON:
			DialogBoxParam(wndInfo.hInstance, MAKEINTRESOURCE(ItemAmountDialog), hWnd, EditMaxAmountDlgProc, (LPARAM)&wndInfo.game);
			break;

		case ERASE_ITEM:
			wndInfo.inventoryList->eraseSelectedItem();
			break;

		case TOGGLE_NOCLIP:
			wndInfo.game.toggleNoclip(SendMessage(wndInfo.noclipCheckbox, BM_GETCHECK, 0, 0) ? true : false);
			break;

		case TOGGLE_ASHLEY:
			wndInfo.game.toggleAshley(SendMessage(wndInfo.ashleyCheckbox, BM_GETCHECK, 0, 0) ? true : false);
			break;

		case TOGGLE_TMP_FIRE_RATE:
			wndInfo.game.toggleFastTmp(SendMessage(wndInfo.tmpCheckbox, BM_GETCHECK, 0, 0) ? true : false);
			break;

		case TOGGLE_EASY_DROPS:
			wndInfo.game.easyDrops(SendMessage(wndInfo.easyDropsCheckbox, BM_GETCHECK, 0, 0) ? true : false);
			break;

		case CHARACTER_COMBO_BOX:
			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				std::uint8_t character = static_cast<std::uint8_t>(SendMessage(wndInfo.characterComboBox, CB_GETCURSEL, 0, 0));

				if (character != CB_ERR)
				{
					wndInfo.game.setCharacter(character);
					SendMessage(wndInfo.costumeComboBox, CB_RESETCONTENT, 0, 0);

					for (const auto &costumeName : wndInfo.game.getCharacterCostumeNames(wndInfo.game.getCharacter()))
						SendMessage(wndInfo.costumeComboBox, CB_ADDSTRING, 0, (LPARAM)costumeName.c_str());
				}
				else
					ErrorBox(hWnd, TEXT("Invalid character"));
			}
			break;

		case COSTUME_COMBO_BOX:
			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				std::uint8_t cbCostume = static_cast<std::uint8_t>(SendMessage(wndInfo.costumeComboBox, CB_GETCURSEL, 0, 0));

				if (wndInfo.game.getCharacter() == 2 && cbCostume == 2)
					wndInfo.game.setCostume(3); //remove this from front end
				else
				{
					if (cbCostume != CB_ERR)
						wndInfo.game.setCostume(cbCostume);
					else
						ErrorBox(hWnd, TEXT("Invalid costume"));
				}
			}
			break;

		case DIFFICULTY_COMBO_BOX:
			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				switch (SendMessage(wndInfo.difficultyComboBox, CB_GETCURSEL, 0, 0))
				{
					case 0:
						wndInfo.game.setDifficulty(Game::Difficulty::AMATEUR);
						break;
					case 1:
						wndInfo.game.setDifficulty(Game::Difficulty::EASY);
						break;
					case 2:
						wndInfo.game.setDifficulty(Game::Difficulty::NORMAL);
						break;
					case 3:
						wndInfo.game.setDifficulty(Game::Difficulty::PROFESSIONAL);
						break;
					default:
						ErrorBox(hWnd, TEXT("Select a difficulty"));
						break;
				}
			}
			break;

		case EDIT_ITEM_STATS:
		{
			auto selectedItem = wndInfo.inventoryList->getSelectedItem();

			if (selectedItem != -1)
			{
				WeaponStatsInfo info(&wndInfo.game, wndInfo.game.getWeaponDataPtr(wndInfo.inventoryList->getItemAddress(selectedItem)->itemId()));
				DialogBoxParam(wndInfo.hInstance, MAKEINTRESOURCE(DLG_WEAPONS_STATS_DIALOG), hWnd, WeaponDlgProc, (LPARAM)&info);
				if (DWORD error = GetLastError())
				{
					String msg = TEXT("Could not create dialog: ");
					msg += std::to_wstring(error);
					ErrorBox(hWnd, msg.c_str());
				}
			}
			else
				ErrorBox(hWnd, TEXT("Select an item first"));
			break;
		}

		case SCENE_SET:
			try {
				wndInfo.game.setScene(std::stoul(GetControlText(wndInfo.sceneCombo).c_str() + 1, nullptr, 16));
			}
			catch (const std::invalid_argument&) {
				ErrorBox(hWnd, TEXT("Invalid scene"));
			}
			break;

		case DOOR_BUTTON:
		{
			auto curSel = SendMessage(wndInfo.doorCombo, CB_GETCURSEL, 0, 0);

			if (curSel != CB_ERR)
			{
				if (static_cast<size_t>(curSel) < wndInfo.game.getDoors().size())
					wndInfo.game.useDoor(wndInfo.game.getDoors()[curSel]);
				else
					ErrorBox(hWnd, TEXT("Invalid door"));
			}
			else
				ErrorBox(hWnd, TEXT("Select a door first"));
			break;
		}

		case SAVE_BUTTON:
			wndInfo.game.openTypewriter(Game::TypewriterMode::SAVE);
			break;

		case MERCHANT_BUTTON:
			wndInfo.game.openMerchant();
			break;

		case KEY_BINDINGS:
			DialogBoxParam(wndInfo.hInstance, MAKEINTRESOURCE(DLG_KEYBINDINGS), hWnd, ConfigDlgProc, NULL);
			break;

		case EXIT:
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
					AppendMenu(hMenu, MF_ENABLED | MF_STRING, MenuIdentifiers::EDIT_ITEM, TEXT("Edit Item"));
					AppendMenu(hMenu, (wndInfo.game.isWeapon(wndInfo.inventoryList->getItemAddress(item.iItem)->itemId()) ? MF_ENABLED : MF_GRAYED) | MF_STRING, MenuIdentifiers::EDIT_ITEM_STATS, TEXT("Edit Item Stats"));
					AppendMenu(hMenu, MF_ENABLED | MF_STRING, MenuIdentifiers::ERASE_ITEM, TEXT("Erase Item"));
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
					toggle = !wndInfo.game.isNoclipOn();
					wndInfo.game.toggleNoclip(toggle);
					SendMessage(wndInfo.noclipCheckbox, BM_SETCHECK, toggle, 0);
					break;
				case KeyBindingsConfig::ASHLEY:
					toggle = !wndInfo.game.isAshleyPresent();
					wndInfo.game.toggleAshley(toggle);
					SendMessage(wndInfo.ashleyCheckbox, BM_SETCHECK, toggle, 0);
					break;
				case KeyBindingsConfig::HEAL:
					wndInfo.game.setHealth(wndInfo.game.getHealthLimit());
					break;
				case KeyBindingsConfig::SAVE:
					wndInfo.game.openTypewriter(Game::TypewriterMode::SAVE);
					break;
				case KeyBindingsConfig::MERCHANT:
					wndInfo.game.openMerchant();
					break;
				case KeyBindingsConfig::MELEE1:
					wndInfo.game.melee(Game::MeleeType::HEAD);
					break;
				case KeyBindingsConfig::MELEE2:
					wndInfo.game.melee(Game::MeleeType::KNEE);
					break;
				case KeyBindingsConfig::INCREASE_ALTITUDE:
					if (auto coordinates = wndInfo.game.getPlayerCoordinates())
					{
						coordinates.value()[1] += 1000.f;
						wndInfo.game.setPlayerCoordinates(coordinates.value());
					}
					break;
				case KeyBindingsConfig::DECREASE_ALTITUDE:
					if (auto coordinates = wndInfo.game.getPlayerCoordinates())
					{
						coordinates.value()[1] -= 1000.f;
						wndInfo.game.setPlayerCoordinates(coordinates.value());
					}
					break;
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