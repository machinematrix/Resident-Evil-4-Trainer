#include "Features.h"
#include <Windows.h>
#ifndef NDEBUG
#include <iostream>
#endif
#include <random>
#include <type_traits>
#include <cstdint>
#include <winsqlite/winsqlite3.h>

namespace
{
	void *cookie = reinterpret_cast<void*>(6);
}

namespace HealthBaseOffsets
{
	enum : std::uint32_t
	{
		Status = 0x20, //0x3: playing; 0x106: pause; 0x4: Changing scene
		Scene = 0x28,
		SceneEntryX = 0x2C, //Player coordinates when the map loads
		SceneEntryY = 0x30,
		SceneEntryZ = 0x34,
		VisibilityFlags = 0x58,
		PlayerX = 0x124,
		PlayerY = 0x128,
		PlayerZ = 0x12C,
		Money = 0x4FA8,
		Health = 0x4FB4,
		MaxHealth = 0x4FB6,
		FirepowerIndex = 0x4FC0,
		FiringSpeed = 0x4FC4,
		Character = 0x4FC8, //0: Leon | 1: Ashley | 2: Ada | 3: HUNK | 4: Krauser | 5: Wesker
		Costume = 0x4FC9, //Leon { 0:Jacket, 1:Normal, 2:Vest, 3:RPD, 4:Mafia } | Ashley { 0:Normal, 1:Popstar, 2:Armor } | Ada { 0:RE2, 1:Spy, 3:Normal } | HUNK { 0:Normal } | Krauser { 0:Normal } | Wesker { 0:Normal }
		AshleyPresent = 0x5028, //0x04000000: Ashley; 0: !Ashley
		Difficulty = 0x847C //0: Amateur?, 3: Easy, 5: Normal, 6: Professional. Firepower is halved in Amateur, but capacity is doubled.
	};
}

namespace PlayerBaseOffsets
{
	enum : std::uint32_t
	{
		EquippedItemId = 0xC,
		EquippedItem = 0x10,
		Inventory = 0x14,
		InventorySize = 0x1C
	};
}

namespace DoorListOffsets
{
	enum : std::uint32_t
	{
		Type = 0x35
	};
}

namespace Characters
{
	enum : std::uint32_t
	{
		Leon,
		Ashley,
		Ada,
		HUNK,
		Krauser,
		Wesker
	};
}

enum class GameState : std::uint32_t
{
	Playing = 0x3,
	ChangingScene = 0x4,
	Pause = 0x106
};

template<typename T>
T getValue(Pointer address)
{
	return *reinterpret_cast<T*>(address);
}

template<typename T>
void setValue(Pointer address, T value)
{
	*reinterpret_cast<T*>(address) = value;
}

template<typename T, size_t sz>
void setValue(Pointer address, const T (&value)[sz])
{
	for (size_t i = 0; i < sz; ++i)
		((T*)address)[i] = value[i];
}

template<typename T>
Pointer pointerPath(Pointer baseAddress, const T& offset)
{
	memcpy(&baseAddress, baseAddress + offset, sizeof(Pointer));
	return baseAddress;
}

template<typename T, typename ...Args>
Pointer pointerPath(Pointer baseAddress, const T& offset, const Args& ...offsets)
{
	memcpy(&baseAddress, baseAddress + offset, sizeof(Pointer));
	return pointerPath(baseAddress, offsets...);
}

Pointer pointerPath(Pointer baseAddress, const std::vector<std::uint64_t>& offsets)
{
	for (auto offset : offsets) {
		memcpy(&baseAddress, baseAddress + offset, sizeof(Pointer));
	}
	return baseAddress;
}

Pointer follow(Pointer instruction)
{
	return instruction + getValue<std::int32_t>(instruction + 1) + 5;
}

template <typename PointerType>
Pointer replaceFunction(Pointer where, PointerType *function)
{
	auto result = follow(where);

	setValue(where + 1, reinterpret_cast<Pointer>(function) - where - 5);
	 
	return result;
}

void __cdecl Game::myGetInventoryModelData(ItemId id, Game::InventoryIconData *result)
{
	static Game *game;

	if (id != ItemId::Invalid)
	{
		std::unique_lock<std::mutex> lck(game->doorVectorMutex);
		auto amountIt = game->itemStackCap.find(id);

		game->getInventoryModelData(id, result);

		if (amountIt != game->itemStackCap.end())
			result->stackLimit(amountIt->second);
	}
	else
		game = reinterpret_cast<Game*>(result);
}

int __cdecl Game::myDropRandomizer(std::uint32_t enemyId, ItemId *outItemId, std::uint32_t *outItemCount, Game *unknownPassZero)
{
	static Game *game;
	bool result = false;

	if (outItemId && outItemCount)
	{
		std::vector<ItemId> candidates = { ItemId::TreasureBoxS, ItemId::FlashGrenade, ItemId::IncendiaryGrenade, ItemId::HandGrenade };
		static std::default_random_engine engine;
		static std::uniform_int_distribution<std::remove_reference<decltype(candidates)>::type::size_type> randomizer(/*ItemIds::MagnumAmmo*/0, /*ItemIds::Mission5TreasureMap*/300);
		Game::InventoryIconData icon;

		if (game->getHealth() <= game->getHealthLimit() / 2) {
			candidates.push_back(ItemId::GreenHerb);
			candidates.push_back(ItemId::FirstAidSpray);
		}

		for (Game::ItemData *item = game->begInventory(), *end = game->endInventory(); item != end; ++item)
		{
			if (item->valid())
			{
				auto &ammoIds = Game::getAmmoItemIds();
				Game::WeaponData *gun = game->getWeaponDataPtr(item->itemId());

				if (gun && std::find(ammoIds.cbegin(), ammoIds.cend(), gun->weaponAmmo()) != ammoIds.end())
				{
					candidates.push_back(gun->weaponAmmo());
					candidates.push_back(gun->weaponAmmo());
					candidates.push_back(gun->weaponAmmo());
					candidates.push_back(gun->weaponAmmo());
				}
			}
		}

		*outItemId = candidates[randomizer(engine) % candidates.size()];
		game->getInventoryModelData(*outItemId, &icon);
		*outItemCount = randomizer(engine) % icon.stackLimit() + 1;

		result = true;
	}
	else
		game = unknownPassZero;

	return result;
}

std::uint32_t __cdecl Game::sceAtHook(void *arg1, void *arg2)
{
	static Game *game;
	std::uint32_t result = 0;

	if (arg2 != cookie)
	{
		result = reinterpret_cast<decltype(sceAtHook)*>(game->sceAtOriginal)(arg1, arg2);
		game->refreshDoorList();
	}
	else
		game = reinterpret_cast<Game*>(arg1);

	return result;
}

Pointer Game::getFirstValidDoor()
{
	Pointer result = nullptr;

	if (result = getValue<Pointer>(doorList))
	{
		result = getValue<Pointer>(result + 4);

		while (!((std::uint32_t)result & 1))
		{
			if (result && getValue<std::uint8_t>(result + DoorListOffsets::Type) == 1) {
				return result;
			}
			if (result)
				result = getValue<Pointer>(result);
			else
				break;
		}
		return nullptr;
	}

	return result;
}

Game::Game()
	:healthBase(patternScan("A1 ????????  83 C0 60  6A 10  50  E8")),
	playerBase(patternScan("B9 ????????  E8 ????????  8B 35 ????????  81")),
	weaponDataIndex(patternScan("B9 ????????  8D A4 24 00000000  66 3B 31  74 10  40")),
	firePowerTable(patternScan("D9 04 8D ????????  D9 5D 08")),
	noclipAddress(patternScan("55 8B EC 53 8B 5D 08 56 8B B3 F4 00 00 00 57 8B 7D 0C 57")),
	doorData(patternScan("????????  6A 0B  75 ??  6A 3A")),
	setScenePtr((decltype(setScenePtr))patternScan("55  8B EC  A1 ????????  53  33 DB  F7 40 54 ????????")),
	sceAtCreateItemAt((decltype(sceAtCreateItemAt))patternScan("55  8B EC  83 EC 0C  57  6A 0D")),
	doorList(patternScan("????????  53  05 8C000000  56  C6 45 CB 00")),
	dropRandomizerHookLocation(patternScan("E8 ????????  83 C4 10  83 F8 01  75 ??  8B 45 08  8B 4D FC")),
	getInventoryModelData((decltype(getInventoryModelData))patternScan("55  8B EC  0FB7 45 ??  3D 0F010000")),
	getModelDataHookLocation(patternScan("E8 ????????  83 C4 08  80 7D 8A 01  74")),
	sceAtHookLocation(patternScan("E8 ????????  8B 0D ????????  A1 ????????  8D 14 09")),
	tmpFireRate(patternScan("D9 05 ????????  D9 45 D4  D8D1  DFE0  DDD9  F6 C4 41")),
	readMinimumHeader((decltype(readMinimumHeader))patternScan("55  8B EC  53  8B 1D ????????  56  68 ????????")),
	loggerFunction(patternScan("E8 ????????  83 C4 08  E8 ????????  53  0FB7 5F 10")),
	loggerFunction2(patternScan("50  68 ????????  6A 00  6A 00  E8 ????????  83 C4 10  33 C0  8B E5  5D  C3")),
	linkedList(patternScan("BB ????????  E8 ????????  89 45 FC  EB 03  8B 45 FC")),
	typewriterProc(patternScan("55  8B EC  A1 ????????  81 88 28500000 00080000")),
	openMerchantPtr(reinterpret_cast<decltype(openMerchantPtr)>(patternScan("55  8B EC  A1 ????????  B9 00000004")))
{
	if ((sqlite3_open("RE4H.db", &database) & 0xFF) == SQLITE_OK)
	{
		std::string_view sql("SELECT * FROM stack_limit");
		sqlite3_stmt *statement = nullptr;

		sqlite3_exec(database, "CREATE TABLE IF NOT EXISTS stack_limit(item_id INTEGER PRIMARY KEY, max_amount INTEGER)", nullptr, nullptr, nullptr);
		
		if ((sqlite3_prepare_v2(database, sql.data(), static_cast<int>(sql.size() + 1), &statement, nullptr) & 0xFF) == SQLITE_OK)
		{
			while ((sqlite3_step(statement) & 0xFF) == SQLITE_ROW)
			{
				int itemId = sqlite3_column_int(statement, 0), limit = sqlite3_column_int(statement, 1);

				itemStackCap[static_cast<ItemId>(itemId)] = limit;
			}

			sqlite3_finalize(statement);
		}
	}

	healthBase = pointerPath(healthBase, 0x1, 0x0);
	playerBase = getValue<Pointer>(playerBase + 1);
	weaponDataIndex = getValue<Pointer>(weaponDataIndex + 1);
	firePowerTable = getValue<Pointer>(firePowerTable + 3);
	doorData = getValue<Pointer>(doorData);
	doorList = getValue<Pointer>(doorList);
	getModelDataHookLocation = follow(getModelDataHookLocation);
	dropRandomizerHookLocation = follow(dropRandomizerHookLocation);
	sceAtHookLocation = follow(sceAtHookLocation);
	loggerFunction = follow(loggerFunction);
	loggerFunction2 = follow(loggerFunction2 + 10);
	tmpFireRate = getValue<Pointer>(tmpFireRate + 2);
	linkedList = getValue<Pointer>(linkedList + 1);

	myDropRandomizer(0, nullptr, nullptr, this);
	myGetInventoryModelData(ItemId::Invalid, (InventoryIconData*)this);
	sceAtHook(this, cookie);

	getModelDataOriginal = replaceFunction(getModelDataHookLocation, myGetInventoryModelData);
	dropRandomizerOriginal = replaceFunction(dropRandomizerHookLocation, myDropRandomizer);
	sceAtOriginal = replaceFunction(sceAtHookLocation, sceAtHook);
	originalLogger = replaceFunction(loggerFunction, loggerFunction);
	originalLogger2 = replaceFunction(loggerFunction2, loggerFunction2);
#ifndef NDEBUG
	using std::cout;
	using std::endl;
	cout << "Health Base: " << (void*)healthBase << endl;
	cout << "Player Base: " << (void*)playerBase << endl;
	cout << "Weapon Data Index: " << (void*)weaponDataIndex << endl;
	cout << "Fire power table: " << (void*)firePowerTable << endl;
	cout << "Noclip code: " << (void*)noclipAddress << endl;
	cout << "Door data: " << (void*)doorData << endl;
	cout << "Door List: " << (void *)doorList << endl;
	cout << "Set Scene: " << (void*)setScenePtr << endl;
	cout << "SceCreateItemAt: " << (void*)sceAtCreateItemAt << endl;
	cout << "Logger Function Jump: " << (void*)loggerFunction << endl;
#endif
}

Game::~Game()
{
	replaceFunction(getModelDataHookLocation, getModelDataOriginal);
	replaceFunction(dropRandomizerHookLocation, dropRandomizerOriginal);
	replaceFunction(sceAtHookLocation, sceAtOriginal);
	replaceFunction(loggerFunction, originalLogger);
	replaceFunction(loggerFunction2, originalLogger2);
	sqlite3_close_v2(database);
}

bool Game::good()
{
	return healthBase
		&& playerBase
		&& weaponDataIndex
		&& firePowerTable
		&& noclipAddress
		&& doorData
		&& doorList
		&& dropRandomizerHookLocation
		&& getModelDataHookLocation
		&& setScenePtr
		&& sceAtCreateItemAt
		&& getInventoryModelData
		&& tmpFireRate
		&& linkedList
		&& readMinimumHeader
		&& typewriterProc
		&& loggerFunction
		&& loggerFunction2
		;
}

void Game::setHealth(std::uint16_t health)
{
	setValue(healthBase + HealthBaseOffsets::Health, health);
}

std::uint16_t Game::getHealth()
{
	return getValue<std::uint16_t>(healthBase + HealthBaseOffsets::Health);
}

void Game::setHealthLimit(std::uint16_t limit)
{
	setValue(healthBase + HealthBaseOffsets::MaxHealth, limit);
}

std::uint16_t Game::getHealthLimit()
{
	return getValue<std::uint16_t>(healthBase + HealthBaseOffsets::MaxHealth);
}

decltype(Game::items)::ValueType Game::getItemName(const decltype(items)::KeyType &id) const
{
	return items.at(id);
}

decltype(Game::items)::KeyType Game::getItemId(const decltype(items)::ValueType &id) const
{
	return items.at(id);
}

Game::ItemData* Game::begInventory() const
{
	return getValue<ItemData*>(playerBase + PlayerBaseOffsets::Inventory);
}

Game::ItemData* Game::endInventory() const
{
	return getValue<ItemData*>(playerBase + PlayerBaseOffsets::Inventory) + getValue<std::uint32_t>(playerBase + PlayerBaseOffsets::InventorySize);
}

Game::ItemData* Game::addItem() const
{
	std::uint32_t invSize = getValue<std::uint32_t>(playerBase + PlayerBaseOffsets::InventorySize);
	ItemData *result = getValue<ItemData*>(playerBase + PlayerBaseOffsets::Inventory), *end = result + invSize;
	
	while (result != end)
	{
		if (!result->valid())
		{ //if we find a free slot
			result->itemId(ItemId::MagnumAmmo);
			result->amount(1);
			result->inventory(0);
			result->valid(1);
			result->firePower(0);
			result->firingSpeed(0);
			result->reloadSpeed(0);
			result->capacity(0);
			result->ammo(0);
			return result;
		}
		++result;
	}
	return nullptr;
}

const std::vector<String>& Game::getItemNames()
{
	static std::vector<String> itemNames;

	if (itemNames.empty())
	{
		itemNames.reserve(items.size());
		for (const auto &item : items)
			itemNames.push_back(item.second);
	}

	return itemNames;
}

bool Game::isAshleyPresent()
{
	return getValue<std::uint32_t>(healthBase + HealthBaseOffsets::AshleyPresent) & 0x04000000;
}

void Game::toggleAshley(bool toggle)
{
	setValue<std::uint32_t>(healthBase + HealthBaseOffsets::AshleyPresent, toggle ? 0x04000000 : 0);
}

void Game::setCharacter(std::uint8_t id)
{
	if (id <= 5) //Check validity
		setValue<std::uint8_t>(healthBase + HealthBaseOffsets::Character, id);
}

std::uint8_t Game::getCharacter()
{
	return getValue<std::uint8_t>(healthBase + HealthBaseOffsets::Character);
}

void Game::setCostume(std::uint8_t id)
{
	using namespace Characters;
	switch (getCharacter())
	{
	case Leon:
		if (id <= 4)
			setValue(healthBase + HealthBaseOffsets::Costume, id);
		break;
	case Ashley:
		if(id <= 2)
			setValue<std::uint8_t>(healthBase + HealthBaseOffsets::Costume, id);
		break;
	case Ada:
		if (id <= 3 && id != 2)
			setValue<std::uint8_t>(healthBase + HealthBaseOffsets::Costume, id);
		break;
	case HUNK:
	case Krauser:
	case Wesker:
		if (!id)
			setValue<std::uint8_t>(healthBase + HealthBaseOffsets::Costume, id);
		break;
	}
}

std::uint8_t Game::getCostume()
{
	return getValue<std::uint8_t>(healthBase + HealthBaseOffsets::Costume);
}

const std::vector<String>& Game::getCharacterCostumeNames(std::uint8_t id)
{
	static const std::vector<String> costumes[7] = {
		{ TEXT("Normal with Jacket"), TEXT("Normal Without Jacket"), TEXT("Normal With Vest"), TEXT("R.P.D"), TEXT("Mafia") }, //Leon
		{ TEXT("Normal"), TEXT("Pop star"), TEXT("Knight") }, //Ashley
		{ TEXT("The Mercenaries"), TEXT("Assignment Ada"), TEXT("Separate Ways") }, //Ada
		{ TEXT("Normal") }, //HUNK
		{ TEXT("Normal") }, //Krauser
		{ TEXT("Normal") } //Wesker
	};

	return costumes[id <= 5 ? id : 6];
}

Game::WeaponData* Game::getWeaponDataPtr(ItemId id) const
{
	WeaponData *result = nullptr, *iter = (WeaponData*)weaponDataIndex;
	while (iter->id() != ItemId::MagnumAmmo)
	{
		if (iter->id() == id) {
			result = iter;
			break;
		}
		++iter;
	}
	return result;
}

void Game::setWeaponDataPtr(WeaponData *target, const WeaponData &source, const float(&newFirepower)[7])
{
	DWORD oldProtect;
	VirtualProtect(target, sizeof(WeaponData), PAGE_READWRITE, &oldProtect);
	
	setFirepowerTableEntry(target->firepowerIndex(), newFirepower);
	for (size_t i = 0; i < WeaponData::capacitySlotCount; ++i) {
		target->capacity(i, source.capacity(i));
	}
	target->weaponAmmo(source.weaponAmmo());
	target->model(source.model());

	VirtualProtect(target, sizeof(WeaponData), oldProtect, &oldProtect);
}

bool Game::isWeapon(ItemId id) const
{
	return getWeaponDataPtr(id) ? true : false;
}

float* Game::getFirepowerTableEntry(std::uint8_t i) const
{
	return (float*)(firePowerTable + i * 7 * sizeof(float));
}

void Game::setFirepowerTableEntry(std::uint8_t i, const float (&newValues)[7])
{
	setValue(firePowerTable + i * 7 * sizeof(float), newValues);
}

const std::vector<ItemId>& Game::getAmmoItemIds()
{
	static const std::vector<ItemId> ammoIds = {		ItemId::MagnumAmmo,
														ItemId::HandgunAmmo,
														ItemId::RifleAmmo,
														ItemId::ShotgunShells,
														ItemId::HandcannonAmmo,
														ItemId::TMPAmmo1,
														ItemId::Arrows,
														ItemId::RifleAmmoInfrared,
														ItemId::ChicagoTypewriterAmmo,
														ItemId::MineDarts,
														ItemId::BowgunBolts };
	return ammoIds;
}

void Game::setMoney(std::uint32_t value)
{
	setValue<std::uint32_t>(healthBase + HealthBaseOffsets::Money, value);
}

std::uint32_t Game::getMoney()
{
	return getValue<std::uint32_t>(healthBase + HealthBaseOffsets::Money);
}

void Game::useDoor(void *doorData)
{
	setScenePtr(doorData); 
}

void Game::refreshDoorList()
{
	std::unique_lock<std::mutex>(doorVectorMutex);
	sceneChanged = true;
	Pointer door = getFirstValidDoor();

	doors.clear();

	if (door)
	{
		while (!((std::uint32_t)door & 1))
		{
			if (getValue<std::uint8_t>(door + 0x35) == 1) { //if it's a door
				doors.push_back(door);
			}
			door = getValue<Pointer>(door);
		}
	}
}

const std::vector<void *>& Game::getDoors()
{
	return doors;
}

bool Game::doorListChanged()
{
	std::unique_lock<std::mutex>(doorVectorMutex);

	if (sceneChanged) {
		sceneChanged = false;
		return true;
	}
	else return false;
}

void Game::setScene(std::uint32_t scene)
{
	float origin[3] = {};
	setValue<std::uint32_t>(healthBase + HealthBaseOffsets::Scene, scene);
	setValue/*<float, 3>*/(healthBase + HealthBaseOffsets::SceneEntryX, origin);
	setValue(healthBase + HealthBaseOffsets::Status, GameState::ChangingScene);
}

std::uint32_t Game::getScene()
{
	return getValue<std::uint32_t>(healthBase + HealthBaseOffsets::Scene);
}

std::array<float, 3> Game::getSceneEntryCoords()
{
	return getValue<std::array<float, 3>>(healthBase + HealthBaseOffsets::SceneEntryX);
}

void Game::setDifficulty(Difficulty value)
{
	setValue<Difficulty>(healthBase + HealthBaseOffsets::Difficulty, value);
}

Game::Difficulty Game::getDifficulty()
{
	return getValue<Difficulty>(healthBase + HealthBaseOffsets::Difficulty);
}

void Game::toggleNoclip(bool toggle)
{
	DWORD oldProtect;

	if (VirtualProtect(noclipAddress, 1, PAGE_EXECUTE_READWRITE, &oldProtect))
	{
		setValue<std::uint8_t>(noclipAddress, toggle ? 0xC3 /*ret*/ : 0x55 /*push ebp*/);
		VirtualProtect(noclipAddress, 1, oldProtect, &oldProtect);
	}
}

bool Game::isNoclipOn()
{
	return getValue<std::uint8_t>(noclipAddress) == 0xC3 /*ret*/ ? true : false;
}

void Game::spawnPickup(float coords[3], std::uint32_t id, std::uint32_t amount)
{
	sceAtCreateItemAt(coords, id, amount, 3, -1, 0, -1);
}

void Game::spawnPickup(std::uint32_t id, std::uint32_t amount)
{
	float coords[3] = { getValue<float>(healthBase + HealthBaseOffsets::PlayerX), getValue<float>(healthBase + HealthBaseOffsets::PlayerY), getValue<float>(healthBase + HealthBaseOffsets::PlayerZ) };
	spawnPickup(coords, id, amount);
}

Game::InventoryIconData Game::getItemDimensions(ItemId id)
{
	InventoryIconData result = {};
	myGetInventoryModelData(id, &result);
	return result;
}

void Game::setMaxItemAmount(ItemId id, std::uint32_t amount)
{
	std::unique_lock<std::mutex> lck(doorVectorMutex);
	std::string_view sql("REPLACE INTO stack_limit(item_id, max_amount) VALUES (?, ?)");
	sqlite3_stmt *statement = nullptr;

	itemStackCap[id] = amount;
	lck.unlock();

	if ((sqlite3_prepare_v2(database, sql.data(), static_cast<int>(sql.size() + 1), &statement, nullptr) & 0xFF) == SQLITE_OK)
	{
		sqlite3_bind_int(statement, 1, static_cast<int>(id));
		sqlite3_bind_int(statement, 2, amount);
		sqlite3_step(statement);
		sqlite3_finalize(statement);
	}
}

void Game::toggleMaxItemAmountHook(bool toggle)
{
	if (toggle)
		replaceFunction(getModelDataHookLocation, myGetInventoryModelData);
	else
		replaceFunction(getModelDataHookLocation, getModelDataOriginal);
}

bool Game::isMaxItemHookEnabled()
{
	return follow(getModelDataHookLocation) != getModelDataOriginal;
}

void Game::toggleFastTmp(bool toggle)
{
	DWORD oldProtect;
	if (VirtualProtect(tmpFireRate, sizeof(float), PAGE_READWRITE, &oldProtect)) {
		*(float *)tmpFireRate = toggle ? 1.5f : 3.0f;
		VirtualProtect(tmpFireRate, sizeof(float), oldProtect, &oldProtect);
	}
}

bool Game::isFastTmpEnabled()
{
	return *(float*)tmpFireRate == 1.5f;
}

void Game::loadSceneFile(const std::string &sceneName)
{
	HANDLE sceneHandle = CreateFileA(sceneName.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (sceneHandle != INVALID_HANDLE_VALUE)
	{
		readMinimumHeader(sceneHandle, nullptr);
		CloseHandle(sceneHandle);
	}
}

void Game::setLoggerCallback(void (__cdecl *callback)(const char*, ...))
{
	replaceFunction(loggerFunction, callback);
	replaceFunction(loggerFunction2, callback);
}

void Game::openTypewriter(TypewriterMode mode)
{
	Pointer node = getValue<Pointer>(linkedList + 0x34);
	node = getValue<Pointer>(node + 0x14);
	node += 0x18;

	setValue<TypewriterMode>(node + 0x14, mode);
	setValue<Pointer>(node + 0x33C, typewriterProc);
	setValue<std::uint8_t>(node + 4, 1);
}

void Game::openMerchant()
{
	openMerchantPtr(0x10, 0);
}

const Bimap<ItemId, String> Game::items = {
	{ ItemId::MagnumAmmo, TEXT("Magnum Ammo") },
	{ ItemId::HandGrenade, TEXT("Hand Grenade") },
	{ ItemId::IncendiaryGrenade, TEXT("Incendiary Grenade") },
	{ ItemId::Matilda, TEXT("Matilda") },
	{ ItemId::HandgunAmmo, TEXT("Handgun Ammo") },
	{ ItemId::FirstAidSpray, TEXT("First Aid Spray") },
	{ ItemId::GreenHerb, TEXT("Green Herb") },
	{ ItemId::RifleAmmo, TEXT("Rifle Ammo") },
	{ ItemId::ChickenEgg, TEXT("Chicken Egg") },
	{ ItemId::BrownChickenEgg, TEXT("Brown Chicken Egg") },
	{ ItemId::GoldChickenEgg, TEXT("Gold Chicken Egg") },
	{ ItemId::AAA, TEXT("aaa") },
	{ ItemId::PlagaSample, TEXT("Plaga Sample") },
	{ ItemId::KrauserKnife, TEXT("Krauser Knife") },
	{ ItemId::FlashGrenade, TEXT("Flash Grenade") },
	{ ItemId::SalazarFamilyInsignia, TEXT("Salazar Family Insignia") },
	{ ItemId::Bowgun, TEXT("Bowgun") },
	{ ItemId::BowgunBolts, TEXT("Bowgun Bolts") },
	{ ItemId::GreenHerbX2, TEXT("Green Herb (X2)") },
	{ ItemId::GreenHerbX3, TEXT("Green Herb (X3)") },
	{ ItemId::MixedHerbsGR, TEXT("Mixed Herbs (G+R)") },
	{ ItemId::MixedHerbsGRY, TEXT("Mixed Herbs (G+R+Y)") },
	{ ItemId::MixedHerbsGY, TEXT("Mixed Herbs (G+Y)") },
	{ ItemId::RocketLauncherSpecial, TEXT("Rocket Launcher (Special)") },
	{ ItemId::ShotgunShells, TEXT("Shotgun Shells") },
	{ ItemId::RedHerb, TEXT("Red Herb") },
	{ ItemId::HandcannonAmmo, TEXT("Handcannon Ammo") },
	{ ItemId::HourglassWGoldDecor, TEXT("Hourglass w/ gold decor") },
	{ ItemId::YellowHerb, TEXT("Yellow Herb") },
	{ ItemId::StoneTablet, TEXT("Stone Tablet") },
	{ ItemId::LionOrnament, TEXT("Lion Ornament") },
	{ ItemId::GoatOrnament, TEXT("Goat Ornament") },
	{ ItemId::TMPAmmo1, TEXT("TMP Ammo") },
	{ ItemId::Punisher, TEXT("Punisher") },
	{ ItemId::PunisherWSilencer, TEXT("Punisher w/ Silencer") },
	{ ItemId::Handgun, TEXT("Handgun") },
	{ ItemId::HandgunWSilencer, TEXT("Handgun w/ Silencer") },
	{ ItemId::Red9, TEXT("Red9") },
	{ ItemId::Red9WStock, TEXT("Red9 w/ Stock") },
	{ ItemId::Blacktail, TEXT("Blacktail") },
	{ ItemId::BlacktailWSilencer, TEXT("Blacktail w/ Silencer") },
	{ ItemId::BrokenButterfly, TEXT("Broken Butterfly") },
	{ ItemId::Killer7, TEXT("Killer7") },
	{ ItemId::Killer7WSilencer, TEXT("Killer7 w/ Silencer") },
	{ ItemId::Shotgun, TEXT("Shotgun") },
	{ ItemId::Striker, TEXT("Striker") },
	{ ItemId::Rifle, TEXT("Rifle") },
	{ ItemId::RifleSemiAuto, TEXT("Rifle (semi-auto)") },
	{ ItemId::TMP, TEXT("TMP") },
	{ ItemId::ActivationKeyBlue, TEXT("Activation Key (Blue)") },
	{ ItemId::TMPWStock, TEXT("TMP w/ Stock") },
	{ ItemId::ActivationKeyRed, TEXT("Activation Key (Red)") },
	{ ItemId::ChicagoTypewriter1, TEXT("Chicago Typewriter") },
	{ ItemId::RocketLauncher, TEXT("Rocket Launcher") },
	{ ItemId::MineThrower, TEXT("Mine Thrower") },
	{ ItemId::Handcannon, TEXT("Handcannon") },
	{ ItemId::CombatKnife, TEXT("Combat Knife") },
	{ ItemId::SerpentOrnament, TEXT("Serpent Ornament") },
	{ ItemId::MoonstoneRightHalf, TEXT("Moonstone (Right half)") },
	{ ItemId::InsigniaKey, TEXT("Insignia Key") },
	{ ItemId::RoundInsignia, TEXT("Round Insignia") },
	{ ItemId::FalseEye, TEXT("False Eye") },
	{ ItemId::CustomTMP, TEXT("Custom TMP") },
	{ ItemId::SilencerHandgun, TEXT("Silencer (Handgun)") },
	{ ItemId::Punisher2, TEXT("Punisher") },
	{ ItemId::PRL412, TEXT("P.R.L. 412") },
	{ ItemId::StockRed9, TEXT("Stock (Red9)") },
	{ ItemId::StockTMP, TEXT("Stock (TMP)") },
	{ ItemId::ScopeRifle, TEXT("Scope (Rifle)") },
	{ ItemId::ScopeSemiAutoRifle, TEXT("Scope (semi-auto Rifle)") },
	{ ItemId::MineDarts, TEXT("Mine-Darts") },
	{ ItemId::Shotgun2, TEXT("Ada's Shotgun") },
	{ ItemId::CaptureLuisCera, TEXT("Capture Luis Sera") },
	{ ItemId::TargetPractice, TEXT("Target Practice") },
	{ ItemId::LuisMemo, TEXT("LuisMemo") },
	{ ItemId::CastellanMemo, TEXT("Castellan Memo") },
	{ ItemId::FemaleIntruder, TEXT("Female Intruder") },
	{ ItemId::ButlerMemo, TEXT("Butler's Memo") },
	{ ItemId::SampleRetrieved, TEXT("Sample Retrieved") },
	{ ItemId::RitualPreparation, TEXT("Ritual Preparation") },
	{ ItemId::LuisMemo2, TEXT("Luis Memo 2") },
	{ ItemId::RifleSemiAutoWInfraredScope, TEXT("Rifle (semi-auto) w/ Infrared Scope") },
	{ ItemId::KrauserBow, TEXT("Krauser's Bow") },
	{ ItemId::ChicagoTypewriter2, TEXT("Chicago Typewriter") },
	{ ItemId::TreasureMapCastle, TEXT("Treasure Map (Castle)") },
	{ ItemId::TreasureMapIsland, TEXT("Treasure Map (Island)") },
	{ ItemId::VelvetBlue, TEXT("Velvet Blue") },
	{ ItemId::Spinel, TEXT("Spinel") },
	{ ItemId::PearlPendant, TEXT("Pearl Pendant") },
	{ ItemId::BrassPocketWatch, TEXT("Brass Pocket Watch") },
	{ ItemId::ElegantHeaddress, TEXT("Elegant Headdress") },
	{ ItemId::AntiquePipe, TEXT("Antique Pipe") },
	{ ItemId::GoldBangleWPearls, TEXT("Gold Bangle w/ Pearls") },
	{ ItemId::AmberRing, TEXT("Amber Ring") },
	{ ItemId::Beerstein, TEXT("Beerstein") },
	{ ItemId::GreenCatEye, TEXT("Green Catseye") },
	{ ItemId::RedCatEye, TEXT("Red Catseye") },
	{ ItemId::YellowCatEye, TEXT("Yellow Catseye") },
	{ ItemId::BeersteinG, TEXT("Beerstein w/ (G)") },
	{ ItemId::BeersteinR, TEXT("Beerstein w/ (R)") },
	{ ItemId::BeersteinY, TEXT("Beerstein w/ (Y)") },
	{ ItemId::BeersteinGR, TEXT("Beerstein w/ (G,R)") },
	{ ItemId::BeersteinGY, TEXT("Beerstein w/ (G,Y)") },
	{ ItemId::BeersteinRY, TEXT("Beerstein w/ (R,Y)") },
	{ ItemId::BeersteinGRY, TEXT("Beerstein w/ (G,R,Y)") },
	{ ItemId::MoonstoneLeftHalf, TEXT("Moonstone (Left Half)") },
	{ ItemId::ChicagoTypewriterAmmo, TEXT("Chicago Typewriter Ammo") },
	{ ItemId::RifleWScope, TEXT("Rifle + Scope") },
	{ ItemId::RifleSemiAutoWScope, TEXT("Rifle (semi-auto) w/ Scope") },
	{ ItemId::InfiniteLauncher, TEXT("Infinite Launcher") },
	{ ItemId::KingGrail, TEXT("King's Grail") },
	{ ItemId::QueenGrail, TEXT("Queen's Grail") },
	{ ItemId::StaffOfRoyalty, TEXT("Staff of Royalty") },
	{ ItemId::GoldBars, TEXT("Gold Bars") },
	{ ItemId::Arrows, TEXT("Arrows") },
	{ ItemId::BonusTime, TEXT("Bonus Time") },
	{ ItemId::EmergencyLockCardKey, TEXT("Emergency Lock Card Key") },
	{ ItemId::BonusPoints, TEXT("Bonus Points") },
	{ ItemId::GreenCatEye2, TEXT("Green Catseye") },
	{ ItemId::Ruby, TEXT("Ruby") },
	{ ItemId::TreasureBoxS, TEXT("Treasure Box S") },
	{ ItemId::TreasureBoxL, TEXT("Treasure Box L") },
	{ ItemId::BlueMoonstone, TEXT("Blue Moonstone") },
	{ ItemId::KeyToTheMine, TEXT("Key To The Mine") },
	{ ItemId::AttachCaseS, TEXT("Attach Case S") },
	{ ItemId::AttachCaseM, TEXT("Attach Case M") },
	{ ItemId::AttachCaseL, TEXT("Attach Case L") },
	{ ItemId::AttachCaseXL, TEXT("Attach Case XL") },
	{ ItemId::GoldenSword, TEXT("Golden Sword") },
	{ ItemId::IronKey, TEXT("Iron Key") },
	{ ItemId::StoneOfSacrifice, TEXT("Stone Of Sacrifice") },
	{ ItemId::StorageRoomCardKey, TEXT("Storage Room Card Key") },
	{ ItemId::FreezerCardKey, TEXT("Freezer Card Key") },
	{ ItemId::PieceOfTheHolyBeastPanther, TEXT("Piece Of The Holy Beast, Panther") },
	{ ItemId::PieceOfTheHolyBeastSerpent, TEXT("Piece Of The Holy Beast, Serpent") },
	{ ItemId::PieceOfTheHolyBeastEagle, TEXT("Piece Of The Holy Beast, Eagle") },
	{ ItemId::JetSkiKey, TEXT("Jet-ski Key") },
	{ ItemId::DirtyPearlPendant, TEXT("Dirty Pearl Pendant") },
	{ ItemId::DirtyBrassPocketWatch, TEXT("Dirty Brass Pocket Watch") },
	{ ItemId::OldKey, TEXT("Old Key") },
	{ ItemId::CampKey, TEXT("Camp Key") },
	{ ItemId::Dynamite, TEXT("Dynamite") },
	{ ItemId::LiftActivationKey, TEXT("Lift Activation Key") },
	{ ItemId::GoldBangle, TEXT("Gold Bangle") },
	{ ItemId::ElegantPerfumeBottle, TEXT("Elegant Perfume Bottle") },
	{ ItemId::MirrorWPearlAndRubies, TEXT("Mirror w/ Pearls & Rubies") },
	{ ItemId::WasteDisposalCardKey, TEXT("Waste Disposal Card Key") },
	{ ItemId::ElegantChessboard, TEXT("Elegant Chessboard") },
	{ ItemId::RiotGun, TEXT("Riot Gun") },
	{ ItemId::BlackBass, TEXT("Black Bass") },
	{ ItemId::HourglassWGoldDecor2, TEXT("Hourglass w/ gold decor") },
	{ ItemId::BlackBassL, TEXT("Black Bass L") },
	{ ItemId::IlluminadosPendant, TEXT("Illuminados Pendant") },
	{ ItemId::RifleWInfraredScope, TEXT("Rifle w/ Infrared Scope") },
	{ ItemId::Crown, TEXT("Crown") },
	{ ItemId::CrownJewel, TEXT("Crown Jewel") },
	{ ItemId::RoyalInsignia, TEXT("Royal Insignia") },
	{ ItemId::CrownWithJewels, TEXT("Crown with Jewels") },
	{ ItemId::CrownWithInsignia, TEXT("Crown with an insignia") },
	{ ItemId::SalazarFamilyCrown, TEXT("Salazar Family Crown") },
	{ ItemId::RifleAmmoInfrared, TEXT("Rifle Ammo (Infrared)") },
	{ ItemId::Emerald, TEXT("Emerald") },
	{ ItemId::BottleCaps, TEXT("Bottle Caps") },
	{ ItemId::GalleryKey, TEXT("Gallery Key") },
	{ ItemId::EmblemRightHalf, TEXT("Emblem (Right Half)") },
	{ ItemId::EmblemLeftHalf, TEXT("Emblem Left Half") },
	{ ItemId::HexagonalEmblem, TEXT("Hexagonal Emblem") },
	{ ItemId::CastleGateKey, TEXT("Castle Gate Key") },
	{ ItemId::MixedHerbsRY, TEXT("Mixed Herbs (R+Y)") },
	{ ItemId::TreasureMapVillage, TEXT("Treasure Map (Village)") },
	{ ItemId::ScopeMineThrower, TEXT("Scope (Mine Thrower)") },
	{ ItemId::MineThrowerPlusScope, TEXT("Mine Thrower + Scope") },
	{ ItemId::PlayingManual1, TEXT("Playing Manual 1") },
	{ ItemId::InfoOnAshley, TEXT("Info on Ashley") },
	{ ItemId::PlayingManual2, TEXT("Playing Manual 2") },
	{ ItemId::AlertOrder, TEXT("Alert Order") },
	{ ItemId::AboutTheBlueMedallions, TEXT("About the Blue Medallions") },
	{ ItemId::ChiefNote, TEXT("Chief's Note") },
	{ ItemId::ClosureOfTheChurch, TEXT("Closure of the Church") },
	{ ItemId::AnonymousLetter, TEXT("Anonymous Letter") },
	{ ItemId::PlayingManual3, TEXT("Playing Manual 3") },
	{ ItemId::SeraAndTheThirdParty, TEXT("Sera and the 3rd Party") },
	{ ItemId::TwoRoutes, TEXT("Two Routes") },
	{ ItemId::VillageLastDefense, TEXT("Village's Last Defense") },
	{ ItemId::ButterflyLamp, TEXT("Butterfly Lamp") },
	{ ItemId::GreenEye, TEXT("Green Eye") },
	{ ItemId::RedEye, TEXT("Red Eye") },
	{ ItemId::BlueEye, TEXT("Blue Eye") },
	{ ItemId::ButterflyLampG, TEXT("Butterfly Lamp w/ G") },
	{ ItemId::ButterflyLampR, TEXT("Butterfly Lamp w/ R") },
	{ ItemId::ButterflyLampB, TEXT("Butterfly Lamp w/ B") },
	{ ItemId::ButterflyLampGR, TEXT("Butterfly Lamp w/ (G,R)") },
	{ ItemId::ButterflyLampGB, TEXT("Butterfly Lamp w/ (G,B)") },
	{ ItemId::ButterflyLampRB, TEXT("Butterfly Lamp w/ (R,B)") },
	{ ItemId::ButterflyLampRGB, TEXT("Butterfly Lamp w/ (R,G,B)") },
	{ ItemId::PrisonKey, TEXT("Prison Key") },
	{ ItemId::PlatinumSword, TEXT("Platinum Sword") },
	{ ItemId::InfraredScope, TEXT("Infrared Scope") },
	{ ItemId::ElegantMask, TEXT("Elegant Mask") },
	{ ItemId::GreenGem, TEXT("Green Gem") },
	{ ItemId::RedGem, TEXT("Red Gem") },
	{ ItemId::PurpleGem, TEXT("Purple Gem") },
	{ ItemId::ElegantMaskG, TEXT("Elegant Mask w/ (G)") },
	{ ItemId::ElegantMaskR, TEXT("Elegant Mask w/ (R)") },
	{ ItemId::ElegantMaskP, TEXT("Elegant Mask w/ (P)") },
	{ ItemId::ElegantMaskGR, TEXT("Elegant Mask w/ (G,R)") },
	{ ItemId::ElegantMaskGP, TEXT("Elegant Mask w/ (G,P)") },
	{ ItemId::ElegantMaskRP, TEXT("Elegant Mask w/ (R,P)") },
	{ ItemId::ElegantMaskRGP, TEXT("Elegant Mask w/ (R,G,P)") },
	{ ItemId::GoldenLynx, TEXT("Golden Lynx") },
	{ ItemId::GreenStoneOfJudgement, TEXT("Green Stone of Judgement") },
	{ ItemId::RedStoneOfFaith, TEXT("Red Stone of Faith") },
	{ ItemId::BlueStoneOfTreason, TEXT("Blue Stone of Treason") },
	{ ItemId::GoldenLynxG, TEXT("Golden Lynx w/ (G)") },
	{ ItemId::GoldenLynxR, TEXT("Golden Lynx w/ (R)") },
	{ ItemId::GoldenLynxB, TEXT("Golden Lynx w/ (B)") },
	{ ItemId::GoldenLynxGR, TEXT("Golden Lynx w/ (G,R)") },
	{ ItemId::GoldenLynxGB, TEXT("Golden Lynx w/ (G,B)") },
	{ ItemId::GoldenLynxRB, TEXT("Golden Lynx w/ (R,B)") },
	{ ItemId::GoldenLynxGRB, TEXT("Golden Lynx w/ (G,R,B)") },
	{ ItemId::LeonWithRocketLauncher, TEXT("Leon w/ rocket launcher") },
	{ ItemId::LeonWithShotgun, TEXT("Leon w/ shotgun") },
	{ ItemId::LeonWithHandgun, TEXT("Leon w/ handgun") },
	{ ItemId::AshleyGraham, TEXT("Ashley Graham") },
	{ ItemId::LuisSera, TEXT("Luis Sera") },
	{ ItemId::DonJose, TEXT("Don Jose") },
	{ ItemId::DonDiego, TEXT("Don Diego") },
	{ ItemId::DonEsteban, TEXT("Don Esteban") },
	{ ItemId::DonManuel, TEXT("Don Manuel") },
	{ ItemId::DrSalvador, TEXT("Dr. Salvador") },
	{ ItemId::Merchant, TEXT("Merchant") },
	{ ItemId::ZealotWithScythe, TEXT("Zealot w/ scythe") },
	{ ItemId::ZealotWithShield, TEXT("Zealot w/ shield") },
	{ ItemId::ZealotWithBowgun, TEXT("Zealot w/ bowgun") },
	{ ItemId::LeaderZealot, TEXT("Leader Zealot") },
	{ ItemId::SoldierWithDynamite, TEXT("Soldier w/ dynamite") },
	{ ItemId::SoldierWithStunRod, TEXT("Soldier w/ stun-rod") },
	{ ItemId::SoldierWithHammer, TEXT("Soldier w/ Hammer") },
	{ ItemId::Isabel, TEXT("Isabel") },
	{ ItemId::Maria, TEXT("Maria") },
	{ ItemId::AdaWong, TEXT("Ada Wong") },
	{ ItemId::BellaSisters, TEXT("Bella Sisters") },
	{ ItemId::DonPedro, TEXT("Don Pedro") },
	{ ItemId::JJ, TEXT("J.J") },
	{ ItemId::LetterFromAda, TEXT("Letter from Ada") },
	{ ItemId::LuisMemo3, TEXT("Luis Memo 3") },
	{ ItemId::PaperAirplane, TEXT("Paper Airplane") },
	{ ItemId::OurPlan, TEXT("Our Plan") },
	{ ItemId::LuisMemo4, TEXT("Luis Memo 4") },
	{ ItemId::KrauserNote, TEXT("Krauser's Note") },
	{ ItemId::LuisMemo5, TEXT("Luis Memo 5") },
	{ ItemId::OurMission, TEXT("Our Mission") },
	{ ItemId::AAA2, TEXT("aaa") },
	{ ItemId::AAA3, TEXT("aaa") },
	{ ItemId::TacticalVest, TEXT("Tactical Vest") },
	{ ItemId::AAA4, TEXT("aaa") },
	{ ItemId::Punisher3, TEXT("Punisher") },
	{ ItemId::Handgun2, TEXT("Handgun") },
	{ ItemId::Shotgun3, TEXT("Shotgun") },
	{ ItemId::MineThrower2, TEXT("Mine Thrower") },
	{ ItemId::Handcannon2, TEXT("Handcannon") },
	{ ItemId::MineThrowerPlusScope2, TEXT("Mine Thrower + Scope") },
	{ ItemId::MissionDirectives1, TEXT("Mission Directives 1") },
	{ ItemId::MissionDirectives2, TEXT("Mission Directives 2") },
	{ ItemId::MissionDirectives3, TEXT("Mission Directives 3") },
	{ ItemId::MissionDirectives4, TEXT("Mission Directives 4") },
	{ ItemId::MissionDirectives5, TEXT("Mission Directives 5") },
	{ ItemId::Mission1TreasureMap, TEXT("Mission 1 Treasure Map") },
	{ ItemId::Mission2TreasureMap, TEXT("Mission 2 Treasure Map") },
	{ ItemId::Mission3TreasureMap, TEXT("Mission 3 Treasure Map") },
	{ ItemId::Mission4TreasureMap, TEXT("Mission 4 Treasure Map") },
	{ ItemId::Mission5TreasureMap, TEXT("Mission 5 Treasure Map") },
};

void Game::ItemData::itemId(ItemId id)
{
	mId = id;
}

ItemId Game::ItemData::itemId() const
{
	return mId;
}

void Game::ItemData::amount(std::uint16_t amount)
{
	mAmount = amount;
}

std::uint16_t Game::ItemData::amount() const
{
	return mAmount;
}

void Game::ItemData::valid(bool valid)
{
	mIsValid = valid;
}

bool Game::ItemData::valid() const
{
	return mIsValid ? true : false;
}

void Game::ItemData::inventory(std::uint8_t inventory)
{
	mInventory = inventory;
}

std::uint8_t Game::ItemData::inventory() const
{
	return mInventory;
}

void Game::ItemData::firingSpeed(std::uint16_t level)
{
	mFiringSpeed = level;
}

std::uint16_t Game::ItemData::firingSpeed() const
{
	return mFiringSpeed;
}

void Game::ItemData::firePower(std::uint16_t level)
{
	mFirePower = level;
}

std::uint16_t Game::ItemData::firePower() const
{
	return mFirePower;
}

void Game::ItemData::capacity(std::uint16_t level)
{
	mCapacity = level;
}

std::uint16_t Game::ItemData::capacity() const
{
	return mCapacity;
}

void Game::ItemData::reloadSpeed(std::uint16_t level)
{
	mReloadSpeed = level;
}

std::uint16_t Game::ItemData::reloadSpeed() const
{
	return mReloadSpeed;
}

void Game::ItemData::ammo(std::uint16_t count)
{
	mAmmo = count << 3;
}

std::uint16_t Game::ItemData::ammo() const
{
	return mAmmo >> 3;
}

void Game::ItemData::posX(std::uint8_t pos)
{
	mPosX = pos;
}

std::uint8_t Game::ItemData::posX() const
{
	return mPosX;
}

void Game::ItemData::posY(std::uint8_t pos)
{
	mPosY = pos;
}

std::uint8_t Game::ItemData::posY() const
{
	return mPosY;
}

void Game::ItemData::rotation(std::uint8_t value)
{
	mRotation = value;
}

std::uint8_t Game::ItemData::rotation() const
{
	return mRotation;
}

void Game::ItemData::inInventory(bool value)
{
	mInInventory = value;
}

std::uint8_t Game::ItemData::inInventory() const
{
	return mInInventory;
}

ItemId Game::WeaponData::id() const
{
	return mId;
}

void Game::WeaponData::firepowerIndex(std::uint8_t position)
{
	mFirepowerIndex = position;
}

std::uint8_t Game::WeaponData::firepowerIndex() const
{
	return mFirepowerIndex;
}

void Game::WeaponData::model(std::uint8_t value)
{
	mModel = value;
}

std::uint8_t Game::WeaponData::model() const
{
	return mModel;
}

void Game::WeaponData::weaponAmmo(ItemId id)
{
	mAmmoItemId = id;
}

ItemId Game::WeaponData::weaponAmmo() const
{
	return mAmmoItemId;
}

void Game::WeaponData::capacity(size_t position, std::uint16_t value)
{
	mCapacityValues[position] = value;
}

std::uint16_t Game::WeaponData::capacity(size_t position) const
{
	return mCapacityValues[position];
}

void Game::InventoryIconData::stackLimit(std::uint8_t value)
{
	mStackLimit = value;
}

std::uint8_t Game::InventoryIconData::stackLimit() const
{
	return mStackLimit;
}