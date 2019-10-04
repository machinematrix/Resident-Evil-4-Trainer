#include "Features.h"
#include <Windows.h>
#ifndef NDEBUG
#include <iostream>
#endif
#include <random>
#include <type_traits>

namespace ItemIds {
	enum : std::uint16_t
	{
		MagnumAmmo,
		HandGrenade,
		IncendiaryGrenade,
		Matilda,
		HandgunAmmo,
		FirstAidSpray,
		GreenHerb,
		RifleAmmo,
		ChickenEgg,
		BrownChickenEgg,
		GoldChickenEgg,
		AAA,
		PlagaSample,
		KrauserKnife,
		FlashGrenade,
		SalazarFamilyInsignia,
		Bowgun,
		BowgunBolts,
		GreenHerbX2,
		GreenHerbX3,
		MixedHerbsGR,
		MixedHerbsGRY,
		MixedHerbsGY,
		RocketLauncherSpecial,
		ShotgunShells,
		RedHerb,
		HandcannonAmmo,
		HourglassWGoldDecor,
		YellowHerb,
		StoneTablet,
		LionOrnament,
		GoatOrnament,
		TMPAmmo1,
		Punisher,
		PunisherWSilencer,
		Handgun,
		HandgunWSilencer,
		Red9,
		Red9WStock,
		Blacktail,
		BlacktailWSilencer,
		BrokenButterfly,
		Killer7,
		Killer7WSilencer,
		Shotgun,
		Striker,
		Rifle,
		RifleSemiAuto,
		TMP,
		ActivationKeyBlue,
		TMPWStock,
		ActivationKeyRed,
		ChicagoTypewriter1,
		RocketLauncher,
		MineThrower,
		Handcannon,
		CombatKnife,
		SerpentOrnament,
		MoonstoneRightHalf,
		InsigniaKey,
		RoundInsignia,
		FalseEye,
		CustomTMP,
		SilencerHandgun,
		Punisher2,
		PRL412,
		StockRed9,
		StockTMP,
		ScopeRifle,
		ScopeSemiAutoRifle,
		MineDarts,
		Shotgun2,
		CaptureLuisCera,
		TargetPractice,
		LuisMemo,
		CastellanMemo,
		FemaleIntruder,
		ButlerMemo,
		SampleRetrieved,
		RitualPreparation,
		LuisMemo2,
		RifleSemiAutoWInfraredScope,
		KrauserBow,
		ChicagoTypewriter2,
		TreasureMapCastle,
		TreasureMapIsland,
		VelvetBlue,
		Spinel,
		PearlPendant,
		BrassPocketWatch,
		ElegantHeaddress,
		AntiquePipe,
		GoldBangleWPearls,
		AmberRing,
		Beerstein,
		GreenCatEye,
		RedCatEye,
		YellowCatEye,
		BeersteinG,
		BeersteinR,
		BeersteinY,
		BeersteinGR,
		BeersteinGY,
		BeersteinRY,
		BeersteinGRY,
		MoonstoneLeftHalf,
		ChicagoTypewriterAmmo,
		RifleWScope,
		RifleSemiAutoWScope,
		InfiniteLauncher,
		KingGrail,
		QueenGrail,
		StaffOfRoyalty,
		GoldBars,
		Arrows,
		BonusTime,
		EmergencyLockCardKey,
		BonusPoints,
		GreenCatEye2,
		Ruby,
		TreasureBoxS,
		TreasureBoxL,
		BlueMoonstone,
		KeyToTheMine,
		AttachCaseS,
		AttachCaseM,
		AttachCaseL,
		AttachCaseXL,
		GoldenSword,
		IronKey,
		StoneOfSacrifice,
		StorageRoomCardKey,
		FreezerCardKey,
		PieceOfTheHolyBeastPanther,
		PieceOfTheHolyBeastSerpent,
		PieceOfTheHolyBeastEagle,
		JetSkiKey,
		DirtyPearlPendant,
		DirtyBrassPocketWatch,
		OldKey,
		CampKey,
		Dynamite,
		LiftActivationKey,
		GoldBangle,
		ElegantPerfumeBottle,
		MirrorWPearlAndRubies,
		WasteDisposalCardKey,
		ElegantChessboard,
		RiotGun,
		BlackBass,
		HourglassWGoldDecor2,
		BlackBassL,
		IlluminadosPendant,
		RifleWInfraredScope,
		Crown,
		CrownJewel,
		RoyalInsignia,
		CrownWithJewels,
		CrownWithInsignia,
		SalazarFamilyCrown,
		RifleAmmoInfrared,
		Emerald,
		BottleCaps,
		GalleryKey,
		EmblemRightHalf,
		EmblemLeftHalf,
		HexagonalEmblem,
		CastleGateKey,
		MixedHerbsRY,
		TreasureMapVillage,
		ScopeMineThrower,
		MineThrowerPlusScope,
		PlayingManual1,
		InfoOnAshley,
		PlayingManual2,
		AlertOrder,
		AboutTheBlueMedallions,
		ChiefNote,
		ClosureOfTheChurch,
		AnonymousLetter,
		PlayingManual3,
		SeraAndTheThirdParty,
		TwoRoutes,
		VillageLastDefense,
		ButterflyLamp,
		GreenEye,
		RedEye,
		BlueEye,
		ButterflyLampG,
		ButterflyLampR,
		ButterflyLampB,
		ButterflyLampGR,
		ButterflyLampGB,
		ButterflyLampRB,
		ButterflyLampRGB,
		PrisonKey,
		PlatinumSword,
		InfraredScope,
		ElegantMask,
		GreenGem,
		RedGem,
		PurpleGem,
		ElegantMaskG,
		ElegantMaskR,
		ElegantMaskP,
		ElegantMaskGR,
		ElegantMaskGP,
		ElegantMaskRP,
		ElegantMaskRGP,
		GoldenLynx,
		GreenStoneOfJudgement,
		RedStoneOfFaith,
		BlueStoneOfTreason,
		GoldenLynxG,
		GoldenLynxR,
		GoldenLynxB,
		GoldenLynxGR,
		GoldenLynxGB,
		GoldenLynxRB,
		GoldenLynxGRB,
		LeonWithRocketLauncher,
		LeonWithShotgun,
		LeonWithHandgun,
		AshleyGraham,
		LuisSera,
		DonJose,
		DonDiego,
		DonEsteban,
		DonManuel,
		DrSalvador,
		Merchant,
		ZealotWithScythe,
		ZealotWithShield,
		ZealotWithBowgun,
		LeaderZealot,
		SoldierWithDynamite,
		SoldierWithStunRod,
		SoldierWithHammer,
		Isabel,
		Maria,
		AdaWong,
		BellaSisters,
		DonPedro,
		JJ,
		LetterFromAda,
		LuisMemo3,
		PaperAirplane,
		OurPlan,
		LuisMemo4,
		KrauserNote,
		LuisMemo5,
		OurMission,
		AAA2,
		AAA3,
		TacticalVest,
		AAA4,
		Punisher3,
		Handgun2,
		Shotgun3,
		MineThrower2,
		Handcannon2,
		MineThrowerPlusScope2,
		MissionDirectives1,
		MissionDirectives2,
		MissionDirectives3,
		MissionDirectives4,
		MissionDirectives5,
		Mission1TreasureMap,
		Mission2TreasureMap,
		Mission3TreasureMap,
		Mission4TreasureMap,
		Mission5TreasureMap,
	};
}

namespace HealthBaseOffsets {
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

namespace PlayerBaseOffsets {
	enum : std::uint32_t
	{
		EquippedItemId = 0xC,
		EquippedItem = 0x10,
		Inventory = 0x14,
		InventorySize = 0x1C
	};
}

namespace DoorListOffsets {
	enum : std::uint32_t
	{
		Type = 0x35
	};
}

namespace Characters {
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

void __cdecl myGetInventoryModelData(std::uint32_t id, Game::InventoryIconData *result)
{
	static Game *game;

	if (id != (~0u))
	{
		std::unique_lock<std::mutex> lck(game->doorVectorMutex);
		auto amountIt = game->itemMaxAmount.find(id);

		game->getInventoryModelData(id, result);

		if (amountIt != game->itemMaxAmount.end()) {
			result->stackLimit = result->stackLimit & 0xFFFFFF00 | (amountIt->second & 0xFF);
		}
	}
	else {
		game = (Game *)result;
	}
}

int __cdecl myDropRandomizer(std::uint32_t enemyId, std::uint32_t *outItemId, std::uint32_t *outItemCount, Game *unknownPassZero)
{
	static Game *game;
	bool result = false;
	using namespace ItemIds;

	if (outItemId && outItemCount)
	{
		std::vector<std::uint16_t> candidates = { TreasureBoxS, FlashGrenade, IncendiaryGrenade, HandGrenade };
		static std::default_random_engine engine;
		static std::uniform_int_distribution<std::remove_reference<decltype(candidates)>::type::size_type> randomizer(ItemIds::MagnumAmmo, ItemIds::Mission5TreasureMap);
		Game::InventoryIconData icon;

		if (game->getHealth() <= game->getHealthLimit() / 2) {
			candidates.push_back(GreenHerb);
			candidates.push_back(FirstAidSpray);
		}

		for (Game::ItemData *item = game->begInventory(), *end = game->endInventory(); item != end; ++item)
		{
			if (item->valid)
			{
				auto &ammoIds = Game::getAmmoItemIds();
				Game::WeaponData *gun = game->getWeaponDataPtr(item->id);

				if (gun && std::find(ammoIds.cbegin(), ammoIds.cend(), gun->ammoItemId) != ammoIds.end())
				{
					candidates.push_back(gun->ammoItemId);
					candidates.push_back(gun->ammoItemId);
					candidates.push_back(gun->ammoItemId);
				}
			}
		}

		*outItemId = candidates[randomizer(engine) % candidates.size()];
		game->getInventoryModelData(*outItemId, &icon);
		*outItemCount = randomizer(engine) % (/*game->getItemDimensions(*outItemId)*/icon.stackLimit) + 1;

		result = true;
	}
	else {
		game = unknownPassZero;
	}

	return result;
}

template<typename T>
inline T Game::getValue(Pointer address) const
{
	return *(T*)(address);
}

template<typename T>
inline void Game::setValue(Pointer address, T value) const
{
	*(T*)(address) = value;
}

template<typename T, size_t sz>
inline void Game::setValue(Pointer address, const T value[sz]) const
{
	for (size_t i = 0; i < sz; ++i) {
		((T*)address)[i] = value[i];
	}
}

template<typename T>
inline Pointer Game::pointerPath(Pointer baseAddress, const T& offset)
{
	memcpy(&baseAddress, baseAddress + offset, sizeof(Pointer));
	return baseAddress;
}

template<typename T, typename ...Args>
Pointer Game::pointerPath(Pointer baseAddress, const T& offset, const Args& ...offsets)
{
	memcpy(&baseAddress, baseAddress + offset, sizeof(Pointer));
	return pointerPath(baseAddress, offsets...);
}

Pointer Game::pointerPath(Pointer baseAddress, const std::vector<std::uint64_t>& offsets)
{
	for (auto offset : offsets) {
		memcpy(&baseAddress, baseAddress + offset, sizeof(Pointer));
	}
	return baseAddress;
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

void Game::setHooks()
{
	std::string code("\x51\x53\xB9\x00\x00\x00\x00\xBB\x00\x00\x00\x00\xFF\xD3\x5B\x59\xC3", 17);
	void *codePointer = (void *)1;
	__asm
	{
		push eax
		mov eax, Game::refreshDoorList
		mov[codePointer], eax
		pop eax;
	};
	setValue<Game *>(&code.front() + 3, this);
	setValue<void *>(&code.front() + 8, codePointer);
	memcpy(refreshDoorsHookLocation, code.c_str(), code.size());

	dropRandomizerOriginal = getValue<Pointer>(dropRandomizerHookLocation + 1);
	setValue<std::uint32_t>(dropRandomizerHookLocation + 1, (Pointer)myDropRandomizer - dropRandomizerHookLocation - 5); //IT SHOULD BE +5, NOT -5, BUT IT CALLS 10 BYTES AHEAD OTHERWISE. SOMETHING'S WRONG

	getModelDataOriginal = getValue<Pointer>(getModelDataHookLocation + 1);
	//getModelDataOriginal = getValue<Pointer>(getModelDataHookLocation + (int)getModelDataOriginal);
	setValue<std::uint32_t>(getModelDataHookLocation + 1, (Pointer)::myGetInventoryModelData - getModelDataHookLocation - 5);
}

void Game::removeHooks()
{
	const char originalCode[] = "\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC";
	memcpy(refreshDoorsHookLocation, originalCode, sizeof(originalCode) - 1);
	setValue<Pointer>(dropRandomizerHookLocation + 1, dropRandomizerOriginal);
	setValue<Pointer>(getModelDataHookLocation + 1, getModelDataOriginal);
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
	refreshDoorsHookLocation(patternScan("7D CC  5F  5E  5B  5D  C3")),
	dropRandomizerHookLocation(patternScan("E8 ????????  83 C4 10  83 F8 01  75 ??  8B 45 08  8B 4D FC")),
	getInventoryModelData((decltype(getInventoryModelData))patternScan("55  8B EC  0FB7 45 ??  3D 0F010000")),
	getModelDataHookLocation(patternScan("E8 ????????  83 C4 08  80 7D 8A 01  74")),
	tmpFireRate(patternScan("D9 05 ????????  D9 45 D4  D8D1  DFE0  DDD9  F6 C4 41")),
	readMinimumHeader((decltype(readMinimumHeader))patternScan("55  8B EC  53  8B 1D ????????  56  68 ????????")),
	loggerFunction(patternScan("E8 ????????  83 C4 08  E8 ????????  53  0FB7 5F 10")),
	linkedList(patternScan("BB ????????  E8 ????????  89 45 FC  EB 03  8B 45 FC")),
	SaveProcedure((decltype(SaveProcedure))patternScan("55  8B EC  56 8B 75 08  8B 46 3C  6A FF")),
	typewriterProc(patternScan("55  8B EC  A1 ????????  81 88 28500000 00080000"))
	//SetPanel((decltype(SetPanel))patternScan("55 8B EC  8B 45 0C  8B 4D 10  8B 55 14  53  56  8B 75 08"))
	//srcSchedulerCpp(patternScan("68 AF000000  68 ????????  6A 01"))
{
#ifndef NDEBUG
	using std::cout;
	using std::endl;
#endif
	healthBase = pointerPath(healthBase, 0x1, 0x0);
	playerBase = getValue<Pointer>(playerBase + 1);
	weaponDataIndex = getValue<Pointer>(weaponDataIndex + 1);
	firePowerTable = getValue<Pointer>(firePowerTable + 3);
	doorData = getValue<Pointer>(doorData);
	doorList = getValue<Pointer>(doorList);
	refreshDoorsHookLocation += 6;
	getModelDataHookLocation += 5 + getValue<std::int32_t>(getModelDataHookLocation + 1);
	dropRandomizerHookLocation += 5 + getValue<std::int32_t>(dropRandomizerHookLocation + 1);
	tmpFireRate += 2;
	tmpFireRate = getValue<Pointer>(tmpFireRate);
	++linkedList = getValue<Pointer>(linkedList);

	loggerFunction += 1; //skip 0xE8
	loggerFunction += getValue<std::int32_t>(loggerFunction) + 4; //+4 because we already skipped the first byte
	loggerFunction += 1; //skip 0xE9

	//srcSchedulerCpp += 6;
	//srcSchedulerCpp = getValue<const char *>((Pointer)srcSchedulerCpp);

	myDropRandomizer(0, nullptr, nullptr, this);
	myGetInventoryModelData(~0u, (InventoryIconData*)this);
	setHooks();

	originalLoggerCallbackOffset = getValue<std::int32_t>(loggerFunction);
#ifndef NDEBUG
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
	setValue<std::int32_t>(loggerFunction, originalLoggerCallbackOffset);
	removeHooks();
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
		&& refreshDoorsHookLocation
		&& dropRandomizerHookLocation
		&& getModelDataHookLocation
		//&& dropRandomizerOriginal
		&& setScenePtr
		&& sceAtCreateItemAt
		&& getInventoryModelData
		&& tmpFireRate
		&& linkedList
		&& readMinimumHeader
		&& SaveProcedure
		&& typewriterProc
		;// &&srcSchedulerCpp;
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

std::vector<Game::ItemData> Game::getInventory() const
{
	std::uint32_t inventorySize = getValue<std::uint32_t>(playerBase + PlayerBaseOffsets::InventorySize);
	std::vector<ItemData> result;
	result.reserve(inventorySize);
	ItemData *item = getValue<ItemData*>(playerBase + PlayerBaseOffsets::Inventory);

	for (unsigned i = 0; i < inventorySize; ++i) {
		result.push_back(item[i]);
	}

	return result;
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
		if (!result->valid)
		{ //if we find a free slot
			result->id = ItemIds::MagnumAmmo;
			result->amount = 1;
			result->valid = 1;
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
		for (const auto &item : items) {
			itemNames.push_back(item.second);
		}
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
	if (id <= 5) { //Check validity
		setValue<std::uint8_t>(healthBase + HealthBaseOffsets::Character, id);
	}
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
	case Leon: //Leon
		if (id <= 4) setValue(healthBase + HealthBaseOffsets::Costume, id);
		break;
	case Ashley: //Ashley
		if(id <= 2) setValue<std::uint8_t>(healthBase + HealthBaseOffsets::Costume, id);
		break;
	case Ada: //Ada
		if (id <= 3 && id != 2) setValue<std::uint8_t>(healthBase + HealthBaseOffsets::Costume, id);
		break;
	case HUNK: //HUNK
	case Krauser: //Krauser
	case Wesker: //Wesker
		if (!id) setValue<std::uint8_t>(healthBase + HealthBaseOffsets::Costume, id);
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

Game::WeaponData* Game::getWeaponDataPtr(std::uint16_t id) const
{
	WeaponData *result = nullptr, *iter = (WeaponData*)weaponDataIndex;
	while (iter->id)
	{
		if (iter->id == id) {
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
	
	setFirepowerTableEntry(target->firepowerIndex, newFirepower);
	std::copy(std::begin(source.capacityValues), std::end(source.capacityValues), std::begin(target->capacityValues));
	target->ammoItemId = source.ammoItemId;
	target->model = source.model;

	VirtualProtect(target, sizeof(WeaponData), oldProtect, &oldProtect);
}

bool Game::isWeapon(std::uint16_t id) const
{
	return getWeaponDataPtr(id) ? true : false;
}

float* Game::getFirepowerTableEntry(std::uint8_t i) const
{
	return (float*)(firePowerTable + i * 7 * sizeof(float));
}

void Game::setFirepowerTableEntry(std::uint8_t i, const float newValues[7])
{
	setValue<float, 7>(firePowerTable + i * 7 * sizeof(float), newValues);
}

const std::vector<std::uint16_t>& Game::getAmmoItemIds()
{
	using namespace ItemIds;
	static const std::vector<std::uint16_t> ammoIds = { MagnumAmmo, HandgunAmmo, RifleAmmo, ShotgunShells, HandcannonAmmo, TMPAmmo1, Arrows, RifleAmmoInfrared, ChicagoTypewriterAmmo, MineDarts, BowgunBolts };
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
			//if (getValue<std::uint32_t>(door + 0x50) == 0x01000000) { //if it's a door
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
	setValue<float, 3>(healthBase + HealthBaseOffsets::SceneEntryX, origin);
	setValue<std::uint32_t>(healthBase + HealthBaseOffsets::Status, 4);
}

std::uint32_t Game::getScene()
{
	return getValue<std::uint32_t>(healthBase + HealthBaseOffsets::Scene);
}

std::array<float, 3> Game::getSceneEntryCoords()
{
	//return std::array<float, 3>{ getValue<float>(healthBase + SceneEntryX), getValue<float>(healthBase + SceneEntryY), getValue<float>(healthBase + SceneEntryZ) };
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
	if (VirtualProtect(noclipAddress, 1, PAGE_EXECUTE_READWRITE, &oldProtect)) {
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
	//sceCreateItemAt(coords, id, amount, -1, -1, 0, -1);
	sceAtCreateItemAt(coords, id, amount, 3, -1, 0, -1);
}

void Game::spawnPickup(std::uint32_t id, std::uint32_t amount)
{
	float coords[3] = { getValue<float>(healthBase + HealthBaseOffsets::PlayerX), getValue<float>(healthBase + HealthBaseOffsets::PlayerY), getValue<float>(healthBase + HealthBaseOffsets::PlayerZ) };
	spawnPickup(coords, id, amount);
}

Game::InventoryIconData Game::getItemDimensions(std::uint16_t id)
{
	InventoryIconData result = {};
	myGetInventoryModelData(id, &result);
	return result;
}

void Game::setMaxItemAmount(std::uint16_t id, std::uint32_t amount)
{
	std::unique_lock<std::mutex> lck(doorVectorMutex);
	itemMaxAmount[id] = amount;
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

auto Game::setLoggerCallback(void (__cdecl *callback)(const char*, ...)) -> void(__cdecl *)(const char *, ...)
{
	void(__cdecl *result)(const char *, ...) = (decltype(result))(loggerFunction + 4 + getValue<std::int32_t>(loggerFunction));

	setValue<std::int32_t>(loggerFunction, (Pointer)callback - loggerFunction - 4);

	return result;
}

void Game::openTypewriter(TypewriterMode mode)
{
	Pointer node = getValue<Pointer>(linkedList + 0x34), secondNode = node;
	node = getValue<Pointer>(node + 0x14);
	node += 0x18;

	setValue<TypewriterMode>(node + 0x14, mode);
	setValue<Pointer>(node + 0x33C, typewriterProc);
	setValue<std::uint8_t>(node + 4, 1);
}

std::ostream& operator<<(std::ostream &os, const Game::ItemData &data)
{
	os << "ID: " << data.id
		<< " | Amount: " << data.amount
		<< " | Valid: " << data.valid
		<< " | Fire Power: " << data.firePower()
		<< " | Firing Speed: " << data.firingSpeed()
		<< " | Reload Speed: " << data.reloadSpeed()
		<< " | Capacity: " << data.capacity()
		<< " | Ammo: " << data.ammo();
	return os;
}

const Bimap<std::uint16_t, String> Game::items = {
	{ ItemIds::MagnumAmmo, TEXT("Magnum Ammo") },
	{ ItemIds::HandGrenade, TEXT("Hand Grenade") },
	{ ItemIds::IncendiaryGrenade, TEXT("Incendiary Grenade") },
	{ ItemIds::Matilda, TEXT("Matilda") },
	{ ItemIds::HandgunAmmo, TEXT("Handgun Ammo") },
	{ ItemIds::FirstAidSpray, TEXT("First Aid Spray") },
	{ ItemIds::GreenHerb, TEXT("Green Herb") },
	{ ItemIds::RifleAmmo, TEXT("Rifle Ammo") },
	{ ItemIds::ChickenEgg, TEXT("Chicken Egg") },
	{ ItemIds::BrownChickenEgg, TEXT("Brown Chicken Egg") },
	{ ItemIds::GoldChickenEgg, TEXT("Gold Chicken Egg") },
	{ ItemIds::AAA, TEXT("aaa") },
	{ ItemIds::PlagaSample, TEXT("Plaga Sample") },
	{ ItemIds::KrauserKnife, TEXT("Krauser Knife") },
	{ ItemIds::FlashGrenade, TEXT("Flash Grenade") },
	{ ItemIds::SalazarFamilyInsignia, TEXT("Salazar Family Insignia") },
	{ ItemIds::Bowgun, TEXT("Bowgun") },
	{ ItemIds::BowgunBolts, TEXT("Bowgun Bolts") },
	{ ItemIds::GreenHerbX2, TEXT("Green Herb (X2)") },
	{ ItemIds::GreenHerbX3, TEXT("Green Herb (X3)") },
	{ ItemIds::MixedHerbsGR, TEXT("Mixed Herbs (G+R)") },
	{ ItemIds::MixedHerbsGRY, TEXT("Mixed Herbs (G+R+Y)") },
	{ ItemIds::MixedHerbsGY, TEXT("Mixed Herbs (G+Y)") },
	{ ItemIds::RocketLauncherSpecial, TEXT("Rocket Launcher (Special)") },
	{ ItemIds::ShotgunShells, TEXT("Shotgun Shells") },
	{ ItemIds::RedHerb, TEXT("Red Herb") },
	{ ItemIds::HandcannonAmmo, TEXT("Handcannon Ammo") },
	{ ItemIds::HourglassWGoldDecor, TEXT("Hourglass w/ gold decor") },
	{ ItemIds::YellowHerb, TEXT("Yellow Herb") },
	{ ItemIds::StoneTablet, TEXT("Stone Tablet") },
	{ ItemIds::LionOrnament, TEXT("Lion Ornament") },
	{ ItemIds::GoatOrnament, TEXT("Goat Ornament") },
	{ ItemIds::TMPAmmo1, TEXT("TMP Ammo") },
	{ ItemIds::Punisher, TEXT("Punisher") },
	{ ItemIds::PunisherWSilencer, TEXT("Punisher w/ Silencer") },
	{ ItemIds::Handgun, TEXT("Handgun") },
	{ ItemIds::HandgunWSilencer, TEXT("Handgun w/ Silencer") },
	{ ItemIds::Red9, TEXT("Red9") },
	{ ItemIds::Red9WStock, TEXT("Red9 w/ Stock") },
	{ ItemIds::Blacktail, TEXT("Blacktail") },
	{ ItemIds::BlacktailWSilencer, TEXT("Blacktail w/ Silencer") },
	{ ItemIds::BrokenButterfly, TEXT("Broken Butterfly") },
	{ ItemIds::Killer7, TEXT("Killer7") },
	{ ItemIds::Killer7WSilencer, TEXT("Killer7 w/ Silencer") },
	{ ItemIds::Shotgun, TEXT("Shotgun") },
	{ ItemIds::Striker, TEXT("Striker") },
	{ ItemIds::Rifle, TEXT("Rifle") },
	{ ItemIds::RifleSemiAuto, TEXT("Rifle (semi-auto)") },
	{ ItemIds::TMP, TEXT("TMP") },
	{ ItemIds::ActivationKeyBlue, TEXT("Activation Key (Blue)") },
	{ ItemIds::TMPWStock, TEXT("TMP w/ Stock") },
	{ ItemIds::ActivationKeyRed, TEXT("Activation Key (Red)") },
	{ ItemIds::ChicagoTypewriter1, TEXT("Chicago Typewriter") },
	{ ItemIds::RocketLauncher, TEXT("Rocket Launcher") },
	{ ItemIds::MineThrower, TEXT("Mine Thrower") },
	{ ItemIds::Handcannon, TEXT("Handcannon") },
	{ ItemIds::CombatKnife, TEXT("Combat Knife") },
	{ ItemIds::SerpentOrnament, TEXT("Serpent Ornament") },
	{ ItemIds::MoonstoneRightHalf, TEXT("Moonstone (Right half)") },
	{ ItemIds::InsigniaKey, TEXT("Insignia Key") },
	{ ItemIds::RoundInsignia, TEXT("Round Insignia") },
	{ ItemIds::FalseEye, TEXT("False Eye") },
	{ ItemIds::CustomTMP, TEXT("Custom TMP") },
	{ ItemIds::SilencerHandgun, TEXT("Silencer (Handgun)") },
	{ ItemIds::Punisher2, TEXT("Punisher") },
	{ ItemIds::PRL412, TEXT("P.R.L. 412") },
	{ ItemIds::StockRed9, TEXT("Stock (Red9)") },
	{ ItemIds::StockTMP, TEXT("Stock (TMP)") },
	{ ItemIds::ScopeRifle, TEXT("Scope (Rifle)") },
	{ ItemIds::ScopeSemiAutoRifle, TEXT("Scope (semi-auto Rifle)") },
	{ ItemIds::MineDarts, TEXT("Mine-Darts") },
	{ ItemIds::Shotgun2, TEXT("Ada's Shotgun") },
	{ ItemIds::CaptureLuisCera, TEXT("Capture Luis Sera") },
	{ ItemIds::TargetPractice, TEXT("Target Practice") },
	{ ItemIds::LuisMemo, TEXT("LuisMemo") },
	{ ItemIds::CastellanMemo, TEXT("Castellan Memo") },
	{ ItemIds::FemaleIntruder, TEXT("Female Intruder") },
	{ ItemIds::ButlerMemo, TEXT("Butler's Memo") },
	{ ItemIds::SampleRetrieved, TEXT("Sample Retrieved") },
	{ ItemIds::RitualPreparation, TEXT("Ritual Preparation") },
	{ ItemIds::LuisMemo2, TEXT("Luis Memo 2") },
	{ ItemIds::RifleSemiAutoWInfraredScope, TEXT("Rifle (semi-auto) w/ Infrared Scope") },
	{ ItemIds::KrauserBow, TEXT("Krauser's Bow") },
	{ ItemIds::ChicagoTypewriter2, TEXT("Chicago Typewriter") },
	{ ItemIds::TreasureMapCastle, TEXT("Treasure Map (Castle)") },
	{ ItemIds::TreasureMapIsland, TEXT("Treasure Map (Island)") },
	{ ItemIds::VelvetBlue, TEXT("Velvet Blue") },
	{ ItemIds::Spinel, TEXT("Spinel") },
	{ ItemIds::PearlPendant, TEXT("Pearl Pendant") },
	{ ItemIds::BrassPocketWatch, TEXT("Brass Pocket Watch") },
	{ ItemIds::ElegantHeaddress, TEXT("Elegant Headdress") },
	{ ItemIds::AntiquePipe, TEXT("Antique Pipe") },
	{ ItemIds::GoldBangleWPearls, TEXT("Gold Bangle w/ Pearls") },
	{ ItemIds::AmberRing, TEXT("Amber Ring") },
	{ ItemIds::Beerstein, TEXT("Beerstein") },
	{ ItemIds::GreenCatEye, TEXT("Green Catseye") },
	{ ItemIds::RedCatEye, TEXT("Red Catseye") },
	{ ItemIds::YellowCatEye, TEXT("Yellow Catseye") },
	{ ItemIds::BeersteinG, TEXT("Beerstein w/ (G)") },
	{ ItemIds::BeersteinR, TEXT("Beerstein w/ (R)") },
	{ ItemIds::BeersteinY, TEXT("Beerstein w/ (Y)") },
	{ ItemIds::BeersteinGR, TEXT("Beerstein w/ (G,R)") },
	{ ItemIds::BeersteinGY, TEXT("Beerstein w/ (G,Y)") },
	{ ItemIds::BeersteinRY, TEXT("Beerstein w/ (R,Y)") },
	{ ItemIds::BeersteinGRY, TEXT("Beerstein w/ (G,R,Y)") },
	{ ItemIds::MoonstoneLeftHalf, TEXT("Moonstone (Left Half)") },
	{ ItemIds::ChicagoTypewriterAmmo, TEXT("Chicago Typewriter Ammo") },
	{ ItemIds::RifleWScope, TEXT("Rifle + Scope") },
	{ ItemIds::RifleSemiAutoWScope, TEXT("Rifle (semi-auto) w/ Scope") },
	{ ItemIds::InfiniteLauncher, TEXT("Infinite Launcher") },
	{ ItemIds::KingGrail, TEXT("King's Grail") },
	{ ItemIds::QueenGrail, TEXT("Queen's Grail") },
	{ ItemIds::StaffOfRoyalty, TEXT("Staff of Royalty") },
	{ ItemIds::GoldBars, TEXT("Gold Bars") },
	{ ItemIds::Arrows, TEXT("Arrows") },
	{ ItemIds::BonusTime, TEXT("Bonus Time") },
	{ ItemIds::EmergencyLockCardKey, TEXT("Emergency Lock Card Key") },
	{ ItemIds::BonusPoints, TEXT("Bonus Points") },
	{ ItemIds::GreenCatEye2, TEXT("Green Catseye") },
	{ ItemIds::Ruby, TEXT("Ruby") },
	{ ItemIds::TreasureBoxS, TEXT("Treasure Box S") },
	{ ItemIds::TreasureBoxL, TEXT("Treasure Box L") },
	{ ItemIds::BlueMoonstone, TEXT("Blue Moonstone") },
	{ ItemIds::KeyToTheMine, TEXT("Key To The Mine") },
	{ ItemIds::AttachCaseS, TEXT("Attach Case S") },
	{ ItemIds::AttachCaseM, TEXT("Attach Case M") },
	{ ItemIds::AttachCaseL, TEXT("Attach Case L") },
	{ ItemIds::AttachCaseXL, TEXT("Attach Case XL") },
	{ ItemIds::GoldenSword, TEXT("Golden Sword") },
	{ ItemIds::IronKey, TEXT("Iron Key") },
	{ ItemIds::StoneOfSacrifice, TEXT("Stone Of Sacrifice") },
	{ ItemIds::StorageRoomCardKey, TEXT("Storage Room Card Key") },
	{ ItemIds::FreezerCardKey, TEXT("Freezer Card Key") },
	{ ItemIds::PieceOfTheHolyBeastPanther, TEXT("Piece Of The Holy Beast, Panther") },
	{ ItemIds::PieceOfTheHolyBeastSerpent, TEXT("Piece Of The Holy Beast, Serpent") },
	{ ItemIds::PieceOfTheHolyBeastEagle, TEXT("Piece Of The Holy Beast, Eagle") },
	{ ItemIds::JetSkiKey, TEXT("Jet-ski Key") },
	{ ItemIds::DirtyPearlPendant, TEXT("Dirty Pearl Pendant") },
	{ ItemIds::DirtyBrassPocketWatch, TEXT("Dirty Brass Pocket Watch") },
	{ ItemIds::OldKey, TEXT("Old Key") },
	{ ItemIds::CampKey, TEXT("Camp Key") },
	{ ItemIds::Dynamite, TEXT("Dynamite") },
	{ ItemIds::LiftActivationKey, TEXT("Lift Activation Key") },
	{ ItemIds::GoldBangle, TEXT("Gold Bangle") },
	{ ItemIds::ElegantPerfumeBottle, TEXT("Elegant Perfume Bottle") },
	{ ItemIds::MirrorWPearlAndRubies, TEXT("Mirror w/ Pearls & Rubies") },
	{ ItemIds::WasteDisposalCardKey, TEXT("Waste Disposal Card Key") },
	{ ItemIds::ElegantChessboard, TEXT("Elegant Chessboard") },
	{ ItemIds::RiotGun, TEXT("Riot Gun") },
	{ ItemIds::BlackBass, TEXT("Black Bass") },
	{ ItemIds::HourglassWGoldDecor2, TEXT("Hourglass w/ gold decor") },
	{ ItemIds::BlackBassL, TEXT("Black Bass L") },
	{ ItemIds::IlluminadosPendant, TEXT("Illuminados Pendant") },
	{ ItemIds::RifleWInfraredScope, TEXT("Rifle w/ Infrared Scope") },
	{ ItemIds::Crown, TEXT("Crown") },
	{ ItemIds::CrownJewel, TEXT("Crown Jewel") },
	{ ItemIds::RoyalInsignia, TEXT("Royal Insignia") },
	{ ItemIds::CrownWithJewels, TEXT("Crown with Jewels") },
	{ ItemIds::CrownWithInsignia, TEXT("Crown with an insignia") },
	{ ItemIds::SalazarFamilyCrown, TEXT("Salazar Family Crown") },
	{ ItemIds::RifleAmmoInfrared, TEXT("Rifle Ammo (Infrared)") },
	{ ItemIds::Emerald, TEXT("Emerald") },
	{ ItemIds::BottleCaps, TEXT("Bottle Caps") },
	{ ItemIds::GalleryKey, TEXT("Gallery Key") },
	{ ItemIds::EmblemRightHalf, TEXT("Emblem (Right Half)") },
	{ ItemIds::EmblemLeftHalf, TEXT("Emblem Left Half") },
	{ ItemIds::HexagonalEmblem, TEXT("Hexagonal Emblem") },
	{ ItemIds::CastleGateKey, TEXT("Castle Gate Key") },
	{ ItemIds::MixedHerbsRY, TEXT("Mixed Herbs (R+Y)") },
	{ ItemIds::TreasureMapVillage, TEXT("Treasure Map (Village)") },
	{ ItemIds::ScopeMineThrower, TEXT("Scope (Mine Thrower)") },
	{ ItemIds::MineThrowerPlusScope, TEXT("Mine Thrower + Scope") },
	{ ItemIds::PlayingManual1, TEXT("Playing Manual 1") },
	{ ItemIds::InfoOnAshley, TEXT("Info on Ashley") },
	{ ItemIds::PlayingManual2, TEXT("Playing Manual 2") },
	{ ItemIds::AlertOrder, TEXT("Alert Order") },
	{ ItemIds::AboutTheBlueMedallions, TEXT("About the Blue Medallions") },
	{ ItemIds::ChiefNote, TEXT("Chief's Note") },
	{ ItemIds::ClosureOfTheChurch, TEXT("Closure of the Church") },
	{ ItemIds::AnonymousLetter, TEXT("Anonymous Letter") },
	{ ItemIds::PlayingManual3, TEXT("Playing Manual 3") },
	{ ItemIds::SeraAndTheThirdParty, TEXT("Sera and the 3rd Party") },
	{ ItemIds::TwoRoutes, TEXT("Two Routes") },
	{ ItemIds::VillageLastDefense, TEXT("Village's Last Defense") },
	{ ItemIds::ButterflyLamp, TEXT("Butterfly Lamp") },
	{ ItemIds::GreenEye, TEXT("Green Eye") },
	{ ItemIds::RedEye, TEXT("Red Eye") },
	{ ItemIds::BlueEye, TEXT("Blue Eye") },
	{ ItemIds::ButterflyLampG, TEXT("Butterfly Lamp w/ G") },
	{ ItemIds::ButterflyLampR, TEXT("Butterfly Lamp w/ R") },
	{ ItemIds::ButterflyLampB, TEXT("Butterfly Lamp w/ B") },
	{ ItemIds::ButterflyLampGR, TEXT("Butterfly Lamp w/ (G,R)") },
	{ ItemIds::ButterflyLampGB, TEXT("Butterfly Lamp w/ (G,B)") },
	{ ItemIds::ButterflyLampRB, TEXT("Butterfly Lamp w/ (R,B)") },
	{ ItemIds::ButterflyLampRGB, TEXT("Butterfly Lamp w/ (R,G,B)") },
	{ ItemIds::PrisonKey, TEXT("Prison Key") },
	{ ItemIds::PlatinumSword, TEXT("Platinum Sword") },
	{ ItemIds::InfraredScope, TEXT("Infrared Scope") },
	{ ItemIds::ElegantMask, TEXT("Elegant Mask") },
	{ ItemIds::GreenGem, TEXT("Green Gem") },
	{ ItemIds::RedGem, TEXT("Red Gem") },
	{ ItemIds::PurpleGem, TEXT("Purple Gem") },
	{ ItemIds::ElegantMaskG, TEXT("Elegant Mask w/ (G)") },
	{ ItemIds::ElegantMaskR, TEXT("Elegant Mask w/ (R)") },
	{ ItemIds::ElegantMaskP, TEXT("Elegant Mask w/ (P)") },
	{ ItemIds::ElegantMaskGR, TEXT("Elegant Mask w/ (G,R)") },
	{ ItemIds::ElegantMaskGP, TEXT("Elegant Mask w/ (G,P)") },
	{ ItemIds::ElegantMaskRP, TEXT("Elegant Mask w/ (R,P)") },
	{ ItemIds::ElegantMaskRGP, TEXT("Elegant Mask w/ (R,G,P)") },
	{ ItemIds::GoldenLynx, TEXT("Golden Lynx") },
	{ ItemIds::GreenStoneOfJudgement, TEXT("Green Stone of Judgement") },
	{ ItemIds::RedStoneOfFaith, TEXT("Red Stone of Faith") },
	{ ItemIds::BlueStoneOfTreason, TEXT("Blue Stone of Treason") },
	{ ItemIds::GoldenLynxG, TEXT("Golden Lynx w/ (G)") },
	{ ItemIds::GoldenLynxR, TEXT("Golden Lynx w/ (R)") },
	{ ItemIds::GoldenLynxB, TEXT("Golden Lynx w/ (B)") },
	{ ItemIds::GoldenLynxGR, TEXT("Golden Lynx w/ (G,R)") },
	{ ItemIds::GoldenLynxGB, TEXT("Golden Lynx w/ (G,B)") },
	{ ItemIds::GoldenLynxRB, TEXT("Golden Lynx w/ (R,B)") },
	{ ItemIds::GoldenLynxGRB, TEXT("Golden Lynx w/ (G,R,B)") },
	{ ItemIds::LeonWithRocketLauncher, TEXT("Leon w/ rocket launcher") },
	{ ItemIds::LeonWithShotgun, TEXT("Leon w/ shotgun") },
	{ ItemIds::LeonWithHandgun, TEXT("Leon w/ handgun") },
	{ ItemIds::AshleyGraham, TEXT("Ashley Graham") },
	{ ItemIds::LuisSera, TEXT("Luis Sera") },
	{ ItemIds::DonJose, TEXT("Don Jose") },
	{ ItemIds::DonDiego, TEXT("Don Diego") },
	{ ItemIds::DonEsteban, TEXT("Don Esteban") },
	{ ItemIds::DonManuel, TEXT("Don Manuel") },
	{ ItemIds::DrSalvador, TEXT("Dr. Salvador") },
	{ ItemIds::Merchant, TEXT("Merchant") },
	{ ItemIds::ZealotWithScythe, TEXT("Zealot w/ scythe") },
	{ ItemIds::ZealotWithShield, TEXT("Zealot w/ shield") },
	{ ItemIds::ZealotWithBowgun, TEXT("Zealot w/ bowgun") },
	{ ItemIds::LeaderZealot, TEXT("Leader Zealot") },
	{ ItemIds::SoldierWithDynamite, TEXT("Soldier w/ dynamite") },
	{ ItemIds::SoldierWithStunRod, TEXT("Soldier w/ stun-rod") },
	{ ItemIds::SoldierWithHammer, TEXT("Soldier w/ Hammer") },
	{ ItemIds::Isabel, TEXT("Isabel") },
	{ ItemIds::Maria, TEXT("Maria") },
	{ ItemIds::AdaWong, TEXT("Ada Wong") },
	{ ItemIds::BellaSisters, TEXT("Bella Sisters") },
	{ ItemIds::DonPedro, TEXT("Don Pedro") },
	{ ItemIds::JJ, TEXT("J.J") },
	{ ItemIds::LetterFromAda, TEXT("Letter from Ada") },
	{ ItemIds::LuisMemo3, TEXT("Luis Memo 3") },
	{ ItemIds::PaperAirplane, TEXT("Paper Airplane") },
	{ ItemIds::OurPlan, TEXT("Our Plan") },
	{ ItemIds::LuisMemo4, TEXT("Luis Memo 4") },
	{ ItemIds::KrauserNote, TEXT("Krauser's Note") },
	{ ItemIds::LuisMemo5, TEXT("Luis Memo 5") },
	{ ItemIds::OurMission, TEXT("Our Mission") },
	{ ItemIds::AAA2, TEXT("aaa") },
	{ ItemIds::AAA3, TEXT("aaa") },
	{ ItemIds::TacticalVest, TEXT("Tactical Vest") },
	{ ItemIds::AAA4, TEXT("aaa") },
	{ ItemIds::Punisher3, TEXT("Punisher") },
	{ ItemIds::Handgun2, TEXT("Handgun") },
	{ ItemIds::Shotgun3, TEXT("Shotgun") },
	{ ItemIds::MineThrower2, TEXT("Mine Thrower") },
	{ ItemIds::Handcannon2, TEXT("Handcannon") },
	{ ItemIds::MineThrowerPlusScope2, TEXT("Mine Thrower + Scope") },
	{ ItemIds::MissionDirectives1, TEXT("Mission Directives 1") },
	{ ItemIds::MissionDirectives2, TEXT("Mission Directives 2") },
	{ ItemIds::MissionDirectives3, TEXT("Mission Directives 3") },
	{ ItemIds::MissionDirectives4, TEXT("Mission Directives 4") },
	{ ItemIds::MissionDirectives5, TEXT("Mission Directives 5") },
	{ ItemIds::Mission1TreasureMap, TEXT("Mission 1 Treasure Map") },
	{ ItemIds::Mission2TreasureMap, TEXT("Mission 2 Treasure Map") },
	{ ItemIds::Mission3TreasureMap, TEXT("Mission 3 Treasure Map") },
	{ ItemIds::Mission4TreasureMap, TEXT("Mission 4 Treasure Map") },
	{ ItemIds::Mission5TreasureMap, TEXT("Mission 5 Treasure Map") },
};