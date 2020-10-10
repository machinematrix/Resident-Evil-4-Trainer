#ifndef __HANDLERS__
#define __HANDLERS__
#include "Utility.h"
#include "resource.h"
#include "Features.h"
#include "InventoryListView.h"
#include "KeyBindingsConfig.h"
#include <cstdarg>

BOOL CALLBACK EditMaxAmountDlgProx(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ItemDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK WeaponDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ConfigDlgProc(HWND, UINT, WPARAM, LPARAM);
typedef std::tuple<Game*, Game::ItemData*> DialogInfo;
typedef std::tuple<Game*, Game::WeaponData*> WeaponStatsInfo;

namespace MenuIdentifiers
{
	enum
	{
		InventoryList,
		EditItem,
		EraseItem,
		AddItem,
		EditItemStats,
		HealthEdit,
		HealthSet,
		HealthGet,
		HealthLimitEdit,
		HealthLimitGet,
		HealthLimitSet,
		ToggleAshley,
		CharacterComboBox,
		CostumeComboBox,
		DifficultyComboBox,
		SceneEdit,
		SceneSet,
		HealthText,
		HealthLimitText,
		SceneText,
		DoorText,
		DoorCombo,
		DoorButton,
		CharacterText,
		CostumeText,
		DifficultyText,
		ToggleNoclip,
		KeyBindings, //invokes dialog box
		Exit,
		MaxAmountEditButton,
		ToggleTMPFireRate,
		SaveButton,
		MerchantButton
	};
}

namespace Controls
{
	enum : std::uint32_t
	{
		Noclip,
		Ashley
	};
}

struct MainWindowInfo
{
	HINSTANCE hInstance; //Needed for DialogBox
	Game cheats;
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
	HWND sceneEdit;
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