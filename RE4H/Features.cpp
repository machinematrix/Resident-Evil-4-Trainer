#include "Features.h"
#include <Windows.h>
#include <random>
#include <type_traits>
#include <cstdint>
#include <regex>
#include <filesystem>
#include <winsqlite/winsqlite3.h>
#ifndef NDEBUG
#include <iostream>
#endif

static_assert(offsetof(Game::Entity, Game::Entity::mVTable) == 0x0, "Bad offset");
static_assert(offsetof(Game::Entity, Game::Entity::mNext) == 0x8, "Bad offset");
static_assert(offsetof(Game::Entity, Game::Entity::mX) == 0x94, "Bad offset");
static_assert(offsetof(Game::Entity, Game::Entity::mY) == 0x98, "Bad offset");
static_assert(offsetof(Game::Entity, Game::Entity::mZ) == 0x9C, "Bad offset");
static_assert(offsetof(Game::Entity, Game::Entity::mRotation) == 0xA4, "Bad offset");
static_assert(offsetof(Game::Entity, Game::Entity::mMeleeFlag) == 0xFC, "Bad offset");
static_assert(offsetof(Game::Entity, Game::Entity::mMovementFlag) == 0xFD, "Bad offset");
static_assert(offsetof(Game::Entity, Game::Entity::mRunningFlag) == 0xFE, "Bad offset");
static_assert(offsetof(Game::Entity, Game::Entity::mAimingFlag) == 0xFF, "Bad offset");
static_assert(offsetof(Game::Entity, Game::Entity::mX2) == 0x110, "Bad offset");
static_assert(offsetof(Game::Entity, Game::Entity::mY2) == 0x114, "Bad offset");
static_assert(offsetof(Game::Entity, Game::Entity::mZ2) == 0x118, "Bad offset");
static_assert(offsetof(Game::Entity, Game::Entity::mHealth) == 0x324, "Bad offset");
static_assert(offsetof(Game::Entity, Game::Entity::mHealthLimit) == 0x326, "Bad offset");

namespace
{
	void *cookie = reinterpret_cast<void*>(6);
	constexpr const char *databaseName = "RE4H.db";
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
		Rotation = 0x38,
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

namespace Character
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

void __cdecl Game::myGetInventoryModelData(ItemId id, Game::InventoryIconData *result)
{
	static Game *game;

	if (id != ItemId::Invalid)
	{
		std::unique_lock<std::mutex> lck(game->mStackCapMutex);
		auto amountIt = game->mItemStackCap.find(id);

		game->mGetInventoryModelData(id, result);

		if (amountIt != game->mItemStackCap.end())
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
		game->mGetInventoryModelData(*outItemId, &icon);
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
		result = reinterpret_cast<decltype(sceAtHook)*>(game->mSceAtOriginal)(arg1, arg2);
		game->refreshDoorList();
		if (game->mDoorListUpdateCallback)
			game->mDoorListUpdateCallback(getValue<std::uint32_t>(game->mHealthBase + HealthBaseOffsets::Scene), game->mDoors);
	}
	else
		game = reinterpret_cast<Game*>(arg1);

	return result;
}

void __cdecl Game::useDoorHook(void *arg, void *arg2)
{
	static Game *game;

	if (arg != cookie)
	{
		sqlite3 *database = nullptr;

		game->mUseDoor(arg, arg2);

		if ((sqlite3_open(databaseName, &database) & 0xFF) == SQLITE_OK)
		{
			std::string_view sql("INSERT INTO scene_entry(scene_id, x, y, z, rotation) VALUES (?, ?, ?, ?, ?)");
			sqlite3_stmt *statement = nullptr;

			if ((sqlite3_prepare_v2(database, sql.data(), static_cast<int>(sql.size() + 1), &statement, nullptr) & 0xFF) == SQLITE_OK)
			{
				sqlite3_bind_int(statement, 1, getValue<int>(game->mHealthBase + HealthBaseOffsets::Scene));
				sqlite3_bind_double(statement, 2, getValue<float>(game->mHealthBase + HealthBaseOffsets::SceneEntryX));
				sqlite3_bind_double(statement, 3, getValue<float>(game->mHealthBase + HealthBaseOffsets::SceneEntryY));
				sqlite3_bind_double(statement, 4, getValue<float>(game->mHealthBase + HealthBaseOffsets::SceneEntryZ));
				sqlite3_bind_double(statement, 5, getValue<float>(game->mHealthBase + HealthBaseOffsets::Rotation));
				sqlite3_step(statement);
				sqlite3_finalize(statement);
			}
		}

		sqlite3_close_v2(database);
	}
	else
		game = static_cast<Game*>(arg2);
}

Pointer Game::getFirstValidDoor()
{
	Pointer result = nullptr;

	if (result = getValue<Pointer>(mDoorList))
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
	:mHealthBase(patternScan("A1 ????????  83 C0 60  6A 10  50  E8")),
	mPlayerBase(patternScan("B9 ????????  E8 ????????  8B 35 ????????  81")),
	mWeaponDataIndex(patternScan("B9 ????????  8D A4 24 00000000  66 3B 31  74 10  40")),
	mFirePowerTable(patternScan("D9 04 8D ????????  D9 5D 08")),
	mNoclipAddress(patternScan("55 8B EC 53 8B 5D 08 56 8B B3 F4 00 00 00 57 8B 7D 0C 57")),
	mDoorData(patternScan("B9 ????????  6A 0B  75 ??  6A 3A")),
	mUseDoor((decltype(mUseDoor))patternScan("55  8B EC  A1 ????????  53  33 DB  F7 40 54 ????????")),
	mSceAtCreateItemAt((decltype(mSceAtCreateItemAt))patternScan("55  8B EC  83 EC 0C  57  6A 0D")),
	mDoorList(patternScan("A1 ????????  53  05 8C000000  56  C6 45 CB 00")),
	mDropRandomizerHookLocation(patternScan("E8 ????????  83 C4 10  83 F8 01  75 ??  8B 45 08  8B 4D FC")),
	mGetInventoryModelData((decltype(mGetInventoryModelData))patternScan("55  8B EC  0FB7 45 ??  3D 0F010000")),
	mGetModelDataHookLocation(patternScan("E8 ????????  83 C4 08  80 7D 8A 01  74")),
	mSceAtHookLocation(patternScan("E8 ????????  8B 0D ????????  A1 ????????  8D 14 09")),
	mTmpFireRate(patternScan("D9 05 ????????  D9 45 D4  D8D1  DFE0  DDD9  F6 C4 41")),
	mReadMinimumHeader((decltype(mReadMinimumHeader))patternScan("55  8B EC  53  8B 1D ????????  56  68 ????????")),
	mLoggerFunction(patternScan("E8 ????????  83 C4 08  E8 ????????  53  0FB7 5F 10")),
	mLoggerFunction2(patternScan("50  68 ????????  6A 00  6A 00  E8 ????????  83 C4 10  33 C0  8B E5  5D  C3")),
	mLinkedList(patternScan("BB ????????  E8 ????????  89 45 FC  EB 03  8B 45 FC")),
	mTypewriterProcedure(patternScan("55  8B EC  A1 ????????  81 88 28500000 00080000")),
	mOpenMerchant(reinterpret_cast<decltype(mOpenMerchant)>(patternScan("55  8B EC  A1 ????????  B9 00000004"))),
	mEntityList(reinterpret_cast<decltype(mEntityList)>(patternScan("8B 35 ????????  85 F6  74 43  8B C6"))),
	mEnemyVTable(patternScan("C7 06 ????????  8B C6  5E  5D  C2 0400  33 C0  5E  5D  C2 0400  CCCCCCCCCCCCCCCCCCCC 55  8B EC  56  8B 75 08  85 F6  74 0D  8B CE  E8 ????????  C7 06 ????????  5E  5D  C3  CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC  C7 05 ???????? ????????  C7 05 ???????? ????????  C3")),
	mMeleeHead(reinterpret_cast<decltype(mMeleeHead)>(patternScan("55  8B EC  A1 ????????  0FB6 80 ????????  83 E8 03"))),
	mMeleeKnee(reinterpret_cast<decltype(mMeleeKnee)>(patternScan("55  8B EC  A1 ????????  80 B8 C84F0000 04"))),
	mMeleeKneeKrauser(reinterpret_cast<decltype(mMeleeKneeKrauser)>(patternScan("55  8B EC  8B 45 08  80 B8 2C030000 00"))),
	mMeleeKneeSuplex(reinterpret_cast<decltype(mMeleeKneeSuplex)>(patternScan("55  8B EC  8B 45 08  80 B8 2C030000 00"))),
	mMelee(reinterpret_cast<decltype(mMelee)>(patternScan("55  8B EC  56  8B 35 ????????  8B CE  E8 ????????  8B 45 0C"))),
	mUseDoorHookLocation(patternScan("8B 04 DD ????????  8B 55 BC"))
{
	sqlite3 *database = nullptr;

	if ((sqlite3_open(databaseName, &database) & 0xFF) == SQLITE_OK)
	{
		std::string_view sql("SELECT * FROM stack_limit");
		sqlite3_stmt *statement = nullptr;

		sqlite3_exec(database, "CREATE TABLE IF NOT EXISTS stack_limit(item_id INTEGER PRIMARY KEY, max_amount INTEGER)", nullptr, nullptr, nullptr);
		sqlite3_exec(database, "CREATE TABLE IF NOT EXISTS scene_entry(scene_id INTEGER NOT NULL, x REAL NOT NULL, y REAL NOT NULL, z REAL NOT NULL, rotation REAL NOT NULL, PRIMARY KEY (scene_id, x, y, z))", nullptr, nullptr, nullptr);
		
		if ((sqlite3_prepare_v2(database, sql.data(), static_cast<int>(sql.size() + 1), &statement, nullptr) & 0xFF) == SQLITE_OK)
		{
			while ((sqlite3_step(statement) & 0xFF) == SQLITE_ROW)
			{
				int itemId = sqlite3_column_int(statement, 0), limit = sqlite3_column_int(statement, 1);

				mItemStackCap[static_cast<ItemId>(itemId)] = limit;
			}

			sqlite3_finalize(statement);
		}
	}

	sqlite3_close_v2(database);

	mHealthBase = pointerPath(mHealthBase, 0x1, 0x0);
	mPlayerBase = getValue<Pointer>(mPlayerBase + 1);
	mWeaponDataIndex = getValue<Pointer>(mWeaponDataIndex + 1);
	mFirePowerTable = getValue<Pointer>(mFirePowerTable + 3);
	mDoorData = getValue<Pointer>(mDoorData + 1);
	mDoorList = getValue<Pointer>(mDoorList + 1);
	mGetModelDataHookLocation = follow(mGetModelDataHookLocation);
	mDropRandomizerHookLocation = follow(mDropRandomizerHookLocation);
	mSceAtHookLocation = follow(mSceAtHookLocation);
	mLoggerFunction = follow(mLoggerFunction);
	mLoggerFunction2 = follow(mLoggerFunction2 + 10);
	mTmpFireRate = getValue<Pointer>(mTmpFireRate + 2);
	mLinkedList = getValue<Pointer>(mLinkedList + 1);
	mPlayerNode = getValue<Entity**>(reinterpret_cast<Pointer>(mEntityList) + 17);
	mEntityList = getValue<Entity**>(reinterpret_cast<Pointer>(mEntityList) + 2);
	mEnemyVTable = getValue<Pointer>(mEnemyVTable + 2);
	mUseDoorHookLocation = getValue<Pointer>(mUseDoorHookLocation + 3) + 1 * 8;

	myDropRandomizer(0, nullptr, nullptr, this);
	myGetInventoryModelData(ItemId::Invalid, (InventoryIconData*)this);
	sceAtHook(this, cookie);
	useDoorHook(cookie, this);

	//Install hooks
	mGetModelDataOriginal = replaceFunction(mGetModelDataHookLocation, myGetInventoryModelData);
	mDropRandomizerOriginal = replaceFunction(mDropRandomizerHookLocation, myDropRandomizer);
	mSceAtOriginal = replaceFunction(mSceAtHookLocation, sceAtHook);
	mOriginalLogger = replaceFunction(mLoggerFunction, mLoggerFunction);
	mOriginalLogger2 = replaceFunction(mLoggerFunction2, mLoggerFunction2);
	setValue(mUseDoorHookLocation, useDoorHook);
#ifndef NDEBUG
	using std::cout;
	using std::endl;
	cout << "Health Base: " << (void*)mHealthBase << endl;
	cout << "Player Base: " << (void*)mPlayerBase << endl;
	cout << "Weapon Data Index: " << (void*)mWeaponDataIndex << endl;
	cout << "Fire power table: " << (void*)mFirePowerTable << endl;
	cout << "Noclip code: " << (void*)mNoclipAddress << endl;
	cout << "Door data: " << (void*)mDoorData << endl;
	cout << "Door List: " << (void *)mDoorList << endl;
	cout << "Set Scene: " << (void*)mUseDoor << endl;
	cout << "SceCreateItemAt: " << (void*)mSceAtCreateItemAt << endl;
	cout << "Logger Function Jump: " << (void*)mLoggerFunction << endl;
#endif
}

Game::~Game()
{
	replaceFunction(mGetModelDataHookLocation, mGetModelDataOriginal);
	replaceFunction(mDropRandomizerHookLocation, mDropRandomizerOriginal);
	replaceFunction(mSceAtHookLocation, mSceAtOriginal);
	replaceFunction(mLoggerFunction, mOriginalLogger);
	replaceFunction(mLoggerFunction2, mOriginalLogger2);
	setValue(mUseDoorHookLocation, mUseDoor);
}

bool Game::good()
{
	return mHealthBase
		&& mPlayerBase
		&& mWeaponDataIndex
		&& mFirePowerTable
		&& mNoclipAddress
		&& mDoorData
		&& mDoorList
		&& mDropRandomizerHookLocation
		&& mGetModelDataHookLocation
		&& mUseDoor
		&& mSceAtCreateItemAt
		&& mGetInventoryModelData
		&& mTmpFireRate
		&& mLinkedList
		&& mReadMinimumHeader
		&& mTypewriterProcedure
		&& mLoggerFunction
		&& mLoggerFunction2
		;
}

void Game::setHealth(std::uint16_t health)
{
	setValue(mHealthBase + HealthBaseOffsets::Health, health);
}

std::uint16_t Game::getHealth()
{
	return getValue<std::uint16_t>(mHealthBase + HealthBaseOffsets::Health);
}

void Game::setHealthLimit(std::uint16_t limit)
{
	setValue(mHealthBase + HealthBaseOffsets::MaxHealth, limit);
}

std::uint16_t Game::getHealthLimit()
{
	return getValue<std::uint16_t>(mHealthBase + HealthBaseOffsets::MaxHealth);
}

decltype(Game::mItems)::mapped_type Game::getItemName(const decltype(mItems)::key_type &id) const
{
	return mItems.at(id);
}

Game::ItemData* Game::begInventory() const
{
	return getValue<ItemData*>(mPlayerBase + PlayerBaseOffsets::Inventory);
}

Game::ItemData* Game::endInventory() const
{
	return getValue<ItemData*>(mPlayerBase + PlayerBaseOffsets::Inventory) + getValue<std::uint32_t>(mPlayerBase + PlayerBaseOffsets::InventorySize);
}

Game::ItemData* Game::addItem() const
{
	std::uint32_t invSize = getValue<std::uint32_t>(mPlayerBase + PlayerBaseOffsets::InventorySize);
	ItemData *result = getValue<ItemData*>(mPlayerBase + PlayerBaseOffsets::Inventory), *end = result + invSize;
	
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
		itemNames.reserve(mItems.size());
		for (const auto &item : mItems)
			itemNames.push_back(item.second);
	}

	return itemNames;
}

bool Game::isAshleyPresent()
{
	return getValue<std::uint32_t>(mHealthBase + HealthBaseOffsets::AshleyPresent) & 0x04000000;
}

void Game::toggleAshley(bool toggle)
{
	setValue<std::uint32_t>(mHealthBase + HealthBaseOffsets::AshleyPresent, toggle ? 0x04000000 : 0);
}

void Game::setCharacter(std::uint8_t id)
{
	if (id <= 5) //Check validity
		setValue<std::uint8_t>(mHealthBase + HealthBaseOffsets::Character, id);
}

std::uint8_t Game::getCharacter()
{
	return getValue<std::uint8_t>(mHealthBase + HealthBaseOffsets::Character);
}

void Game::setCostume(std::uint8_t id)
{
	using namespace Character;
	switch (getCharacter())
	{
	case Leon:
		if (id <= 4)
			setValue(mHealthBase + HealthBaseOffsets::Costume, id);
		break;
	case Ashley:
		if(id <= 2)
			setValue<std::uint8_t>(mHealthBase + HealthBaseOffsets::Costume, id);
		break;
	case Ada:
		if (id <= 3 && id != 2)
			setValue<std::uint8_t>(mHealthBase + HealthBaseOffsets::Costume, id);
		break;
	case HUNK:
	case Krauser:
	case Wesker:
		if (!id)
			setValue<std::uint8_t>(mHealthBase + HealthBaseOffsets::Costume, id);
		break;
	}
}

std::uint8_t Game::getCostume()
{
	return getValue<std::uint8_t>(mHealthBase + HealthBaseOffsets::Costume);
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
	WeaponData *result = nullptr, *iter = (WeaponData*)mWeaponDataIndex;
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
	return (float*)(mFirePowerTable + i * 7 * sizeof(float));
}

void Game::setFirepowerTableEntry(std::uint8_t i, const float (&newValues)[7])
{
	setValue(mFirePowerTable + i * 7 * sizeof(float), newValues);
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
	setValue<std::uint32_t>(mHealthBase + HealthBaseOffsets::Money, value);
}

std::uint32_t Game::getMoney()
{
	return getValue<std::uint32_t>(mHealthBase + HealthBaseOffsets::Money);
}

void Game::useDoor(void *doorData)
{
	useDoorHook(doorData, nullptr);
}

void Game::refreshDoorList()
{
	mSceneChanged = true;
	Pointer door = getFirstValidDoor();

	mDoors.clear();

	if (door)
	{
		while (!((std::uint32_t)door & 1))
		{
			if (getValue<std::uint8_t>(door + 0x35) == 1) //if it's a door
				mDoors.push_back(door);

			door = getValue<Pointer>(door);
		}
	}
}

const std::vector<void *> Game::getDoors()
{
	return mDoors;
}

void Game::setScene(std::uint32_t scene)
{
	float entry[4] = {};
	sqlite3 *database = nullptr;
	
	setValue<std::uint32_t>(mHealthBase + HealthBaseOffsets::Scene, scene);

	if ((sqlite3_open(databaseName, &database) & 0xFF) == SQLITE_OK)
	{
		std::string_view sql("SELECT x, y, z, rotation FROM scene_entry WHERE scene_id = ?");
		sqlite3_stmt *statement = nullptr;

		if ((sqlite3_prepare_v2(database, sql.data(), static_cast<int>(sql.size() + 1), &statement, nullptr) & 0xFF) == SQLITE_OK)
		{
			sqlite3_bind_int(statement, 1, scene);

			if ((sqlite3_step(statement) & 0xFF) == SQLITE_ROW)
			{
				entry[0] = static_cast<float>(sqlite3_column_double(statement, 0));
				entry[1] = static_cast<float>(sqlite3_column_double(statement, 1));
				entry[2] = static_cast<float>(sqlite3_column_double(statement, 2));
				entry[3] = static_cast<float>(sqlite3_column_double(statement, 3));
			}

			sqlite3_finalize(statement);
		}
	}

	sqlite3_close_v2(database);
	setValue(mHealthBase + HealthBaseOffsets::SceneEntryX, entry);
	setValue(mHealthBase + HealthBaseOffsets::Status, GameState::ChangingScene);
}

std::uint32_t Game::getScene()
{
	return getValue<std::uint32_t>(mHealthBase + HealthBaseOffsets::Scene);
}

std::array<float, 3> Game::getSceneEntryCoords()
{
	return getValue<std::array<float, 3>>(mHealthBase + HealthBaseOffsets::SceneEntryX);
}

void Game::setDifficulty(Difficulty value)
{
	setValue<Difficulty>(mHealthBase + HealthBaseOffsets::Difficulty, value);
}

Game::Difficulty Game::getDifficulty()
{
	return getValue<Difficulty>(mHealthBase + HealthBaseOffsets::Difficulty);
}

void Game::toggleNoclip(bool toggle)
{
	DWORD oldProtect;

	if (VirtualProtect(mNoclipAddress, 1, PAGE_EXECUTE_READWRITE, &oldProtect))
	{
		setValue<std::uint8_t>(mNoclipAddress, toggle ? 0xC3 /*ret*/ : 0x55 /*push ebp*/);
		VirtualProtect(mNoclipAddress, 1, oldProtect, &oldProtect);
	}
}

bool Game::isNoclipOn()
{
	return getValue<std::uint8_t>(mNoclipAddress) == 0xC3 /*ret*/ ? true : false;
}

void Game::spawnPickup(float coords[3], std::uint32_t id, std::uint32_t amount)
{
	mSceAtCreateItemAt(coords, id, amount, 3, -1, 0, -1);
}

void Game::spawnPickup(std::uint32_t id, std::uint32_t amount)
{
	float coords[3] = { getValue<float>(mHealthBase + HealthBaseOffsets::PlayerX), getValue<float>(mHealthBase + HealthBaseOffsets::PlayerY), getValue<float>(mHealthBase + HealthBaseOffsets::PlayerZ) };
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
	std::unique_lock<std::mutex> lck(mStackCapMutex);
	std::string_view sql("REPLACE INTO stack_limit(item_id, max_amount) VALUES (?, ?)");
	sqlite3 *database = nullptr;

	mItemStackCap[id] = amount;
	lck.unlock();

	if ((sqlite3_open(databaseName, &database) & 0xFF) == SQLITE_OK)
	{
		sqlite3_stmt *statement = nullptr;

		if ((sqlite3_prepare_v2(database, sql.data(), static_cast<int>(sql.size() + 1), &statement, nullptr) & 0xFF) == SQLITE_OK)
		{
			sqlite3_bind_int(statement, 1, static_cast<int>(id));
			sqlite3_bind_int(statement, 2, amount);
			sqlite3_step(statement);
			sqlite3_finalize(statement);
		}
	}

	sqlite3_close_v2(database);
}

void Game::toggleMaxItemAmountHook(bool toggle)
{
	if (toggle)
		replaceFunction(mGetModelDataHookLocation, myGetInventoryModelData);
	else
		replaceFunction(mGetModelDataHookLocation, mGetModelDataOriginal);
}

bool Game::isMaxItemHookEnabled()
{
	return follow(mGetModelDataHookLocation) != mGetModelDataOriginal;
}

void Game::toggleFastTmp(bool toggle)
{
	DWORD oldProtect;

	if (VirtualProtect(mTmpFireRate, sizeof(float), PAGE_READWRITE, &oldProtect))
	{
		*(float *)mTmpFireRate = toggle ? 1.5f : 3.0f;
		VirtualProtect(mTmpFireRate, sizeof(float), oldProtect, &oldProtect);
	}
}

bool Game::isFastTmpEnabled()
{
	return *(float*)mTmpFireRate == 1.5f;
}

void Game::loadSceneFile(const std::string &sceneName)
{
	HANDLE sceneHandle = CreateFileA(sceneName.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (sceneHandle != INVALID_HANDLE_VALUE)
	{
		mReadMinimumHeader(sceneHandle, nullptr);
		CloseHandle(sceneHandle);
	}
}

void Game::setLoggerCallback(void (__cdecl *callback)(const char*, const char*, const char*, ...))
{
	replaceFunction(mLoggerFunction, callback);
	replaceFunction(mLoggerFunction2, callback);
}

void Game::openTypewriter(TypewriterMode mode)
{
	Pointer node = getValue<Pointer>(mLinkedList + 0x34);
	node = getValue<Pointer>(node + 0x14);
	node += 0x18;

	setValue<TypewriterMode>(node + 0x14, mode);
	setValue<Pointer>(node + 0x33C, mTypewriterProcedure);
	setValue<std::uint8_t>(node + 4, 1);
}

void Game::openMerchant()
{
	mOpenMerchant(0x10, 0);
}

void Game::melee(MeleeType type)
{
	if (Entity* playerEntity = *mPlayerNode)
	{
		auto character = getCharacter();
		
		if (character == Character::Krauser) //Game crashes when doing this with Krauser for some reason.
			return;

		for (Entity *node = *mEntityList; node; node = node->mNext)
		{
			if (node->mVTable == mEnemyVTable)
			{
				float rotation = playerEntity->mRotation;
				bool freezeRotation = true;

				switch (type)
				{
					case MeleeType::HEAD:
						if (character == Character::HUNK) //If melee is neck breaker
							if (node->mHealth) //and the enemy is alive
								freezeRotation = false; //don't freeze rotation
							else
								continue; //else, look for a new enemy
						mMeleeHead(node, nullptr);
						break;
					case MeleeType::KNEE:
						switch (character)
						{
							case Character::Krauser:
								mMeleeKneeKrauser(node, nullptr);
								break;
							case Character::Leon:
								if (node->mHealth)
									freezeRotation = false;
								else
									continue;
								mMeleeKneeSuplex(node, nullptr);
								break;
							default:
								mMeleeKnee(node, nullptr);
						}
						break;
				}

				if (freezeRotation)
				{
					auto freezeRotation = [](Entity *playerEntity, float rotation) {
						while (playerEntity->mMeleeFlag == 4)
							playerEntity->mRotation = rotation, Sleep(5);
					};

					std::thread(freezeRotation, playerEntity, rotation).detach();
				}

				break;
			}
		}
	}
}

void Game::setPlayerCoordinates(const std::array<float, 3> &coordinates)
{
	if (Entity *playerEntity = *mPlayerNode)
		playerEntity->mX = coordinates[0], playerEntity->mY = coordinates[1], playerEntity->mZ = coordinates[2];
}

std::optional<std::array<float, 3>> Game::getPlayerCoordinates()
{
	std::optional<std::array<float, 3>> result;

	if (Entity *playerEntity = *mPlayerNode)
		result = decltype(result)::value_type{ playerEntity->mX, playerEntity->mY, playerEntity->mZ };

	return result;
}

std::vector<std::wstring> Game::getSceneFileNames()
{
	static const std::wregex sceneFileName(L"r([[:xdigit:]]{3})\\.udas\\.lfs");
	std::vector<std::wstring> result;

	for (std::string_view directory : { "St0", "St1", "St2", "St3", "St4", "St5", "St6", "St7" })
	{
		std::string path("BIO4/");

		path += directory;
		for (std::filesystem::directory_iterator it(path), end; it != end; ++it)
		{
			std::wstring name = it->path().filename().wstring();

			if (std::regex_match(name, sceneFileName))
				result.push_back(name);
		}
	}

	std::sort(result.begin(), result.end(), [](const std::wstring &lhs, const std::wstring &rhs) -> bool { return std::stoi(lhs.c_str() + 1, nullptr, 16) < std::stoi(rhs.c_str() + 1, nullptr, 16); });

	return result;
}

void Game::setDoorListUpdateCallback(decltype(mDoorListUpdateCallback) callback)
{
	mDoorListUpdateCallback = callback;
}

const std::map<ItemId, String> Game::mItems = {
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