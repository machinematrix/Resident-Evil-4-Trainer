#ifndef __HANDLERS__
#define __HANDLERS__
#include "Utility.h"
#include "resource.h"
#include "Features.h"
#include "InventoryListView.h"
#include "KeyBindingsConfig.h"
#include <cstdarg>

BOOL CALLBACK EditMaxAmountDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ItemDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK WeaponDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ConfigDlgProc(HWND, UINT, WPARAM, LPARAM);
//typedef std::tuple<Game*, Game::ItemData*> DialogInfo;
//typedef std::tuple<Game*, Game::WeaponData*> WeaponStatsInfo;

namespace MenuIdentifiers
{
	enum
	{
		InventoryList,
		EDIT_ITEM,
		ERASE_ITEM,
		ADD_ITEM,
		EDIT_ITEM_STATS,
		HealthEdit,
		HEALTH_SET,
		HEALTH_GET,
		HealthLimitEdit,
		HEALTH_LIMIT_GET,
		HEALTH_LIMIT_SET,
		TOGGLE_ASHLEY,
		CHARACTER_COMBO_BOX,
		COSTUME_COMBO_BOX,
		DIFFICULTY_COMBO_BOX,
		SceneCombo,
		SCENE_SET,
		HealthText,
		HealthLimitText,
		SceneText,
		DoorText,
		DoorCombo,
		DOOR_BUTTON,
		CharacterText,
		CostumeText,
		DifficultyText,
		TOGGLE_NOCLIP,
		KEY_BINDINGS, //invokes dialog box
		EXIT,
		MAX_AMOUNT_EDIT_BUTTON,
		TOGGLE_TMP_FIRE_RATE,
		TOGGLE_EASY_DROPS,
		TOGGLE_RADIO,
		SAVE_BUTTON,
		MERCHANT_BUTTON
	};
}

struct MainWindowInfo
{
	HINSTANCE hInstance; //Needed for DialogBox
	HWND healthSet;
	HWND healthGet;
	HWND healthEdit;
	HWND healthLimitSet;
	HWND healthLimitGet;
	HWND healthLimitEdit;
	HWND addItem;
	HWND ashleyCheckbox;
	HWND characterComboBox;
	HWND costumeComboBox;
	HWND sceneCombo;
	HWND sceneSet;
	HWND characterText;
	HWND costumeText;
	HWND difficultyComboBox;
	HWND difficultyText;
	HWND healthText;
	HWND healthLimitText;
	HWND sceneText;
	HWND noclipCheckbox;
	HWND doorText;
	HWND doorCombo;
	HWND doorButton;
	HWND maxAmountEditButton;
	HWND tmpCheckbox;
	HWND easyDropsCheckbox;
	HWND radioCheckbox;
	HWND saveButton;
	HWND merchantButton;
	HHOOK kbHook;
	std::unique_ptr<InventoryListView> inventoryList;
};

void onWmCreate(HWND hWnd, WPARAM wParam, LPARAM lParam, MainWindowInfo &wndInfo);
void onWmSize(HWND hWnd, WPARAM wParam, LPARAM lParam, MainWindowInfo &wndInfo);
void onWmTimer(HWND hWnd, WPARAM wParam, LPARAM lParam, MainWindowInfo &wndInfo);
void onWmCommand(HWND hWnd, WPARAM wParam, LPARAM lParam, MainWindowInfo &wndInfo);
void onWmNotify(HWND hWnd, WPARAM wParam, LPARAM lParam, MainWindowInfo &wndInfo);
void onWmKeydown(HWND hWnd, WPARAM wParam, LPARAM lParam, MainWindowInfo &wndInfo);
void onWmDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam, MainWindowInfo &wndInfo);

#endif