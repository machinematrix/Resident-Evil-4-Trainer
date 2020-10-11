#ifndef __HACKS__
#define __HACKS__
#include "Memory.h"
#include "Bimap.h"
#include <vector>
#include <array>
#include <mutex>
#include <map>

#ifdef UNICODE
typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t>> String;
#else
typedef std::basic_string<char, std::char_traits<char>, std::allocator<char>> String;
#endif

enum class ItemId : std::uint16_t
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
	Invalid = 0xFFFF
};

class Game
{
public:
	class ItemData;
	class WeaponData;
	class InventoryIconData;
private:
	//friend void __cdecl myGetInventoryModelData(ItemId, Game::InventoryIconData*);
	//friend int __cdecl myDropRandomizer(std::uint32_t, ItemId*, std::uint32_t*, Game*);

	static const Bimap<ItemId, String> items;
	std::mutex doorVectorMutex;
	bool sceneChanged = false;
	Pointer healthBase;
	Pointer playerBase;
	Pointer weaponDataIndex;
	Pointer firePowerTable;
	Pointer noclipAddress;
	Pointer doorData;
	Pointer doorList; //at dereference, then at +0x10, first four bits are door index * 2
	Pointer refreshDoorsHookLocation;
	Pointer dropRandomizerHookLocation;
	Pointer dropRandomizerOriginal = nullptr;
	Pointer getModelDataHookLocation;
	Pointer getModelDataOriginal = nullptr;
	Pointer sceAtHookLocation;
	Pointer sceAtOriginal = nullptr;
	Pointer tmpFireRate;
	Pointer loggerFunction;
	Pointer loggerFunction2;
	Pointer linkedList;
	Pointer typewriterProc;
	//Pointer merchantProc;

	std::int32_t originalLoggerCallbackOffset, originalLogger2CallbackOffset;
	std::vector<void*> doors;
	std::map<ItemId, std::uint32_t> itemStackCap;
	
	void(__cdecl *setScenePtr)(void*); //first parameter is a pointer to a 312 byte (0x138) structure
	void(__cdecl *sceAtCreateItemAt)(float coords[3], int32_t itemId, int32_t amount, int32_t /*3 for treasures*/, int32_t, int32_t, int32_t);
	void(__cdecl *getInventoryModelData)(ItemId id, InventoryIconData *result);
	std::uint32_t(__cdecl *readMinimumHeader)(void *sceneHandle, void *unknown);
	void(__cdecl *openMerchantPtr)(std::int32_t, std::int32_t);
	
	static void __cdecl myGetInventoryModelData(ItemId, Game::InventoryIconData*);
	static int __cdecl myDropRandomizer(std::uint32_t, ItemId*, std::uint32_t*, Game*);
	static std::uint32_t __cdecl sceAtHook(std::uint32_t, std::uint32_t);
	Pointer getFirstValidDoor();
	void setHooks();
	void removeHooks();
public:
	enum class Difficulty : std::uint32_t { AMATEUR, EASY = 3, NORMAL = 5, PROFESSIONAL = 6 };
	enum class TypewriterMode : std::uint32_t { LOAD, SAVE };
	Game();
	~Game();
	bool good();

	void setHealth(std::uint16_t health);
	std::uint16_t getHealth();
	void setHealthLimit(std::uint16_t limit);
	std::uint16_t getHealthLimit();

	decltype(items)::ValueType getItemName(const decltype(items)::KeyType &id) const; //throws out_of_range
	decltype(items)::KeyType getItemId(const decltype(items)::ValueType &id) const; //throws out_of_range
	ItemData* begInventory() const;
	ItemData* endInventory() const;
	ItemData* addItem() const;
	const std::vector<String>& getItemNames();

	bool isAshleyPresent();
	void toggleAshley(bool toggle);

	//0: Leon | 1: Ashley | 2: Ada | 3: HUNK | 4: Krauser | 5: Wesker
	void setCharacter(std::uint8_t id);
	std::uint8_t getCharacter();
	void setCostume(std::uint8_t id);
	std::uint8_t getCostume();
	const std::vector<String>& getCharacterCostumeNames(std::uint8_t id);

	WeaponData* getWeaponDataPtr(ItemId id) const;
	void setWeaponDataPtr(WeaponData *target, const WeaponData &source, const float(&newFirepower)[7]);
	bool isWeapon(ItemId id) const;
	float* getFirepowerTableEntry(std::uint8_t i) const;
	void setFirepowerTableEntry(std::uint8_t i, const float (&newValues)[7]);
	static const std::vector<ItemId>& getAmmoItemIds();

	void setMoney(std::uint32_t value);
	std::uint32_t getMoney();

	void useDoor(void *doorData);
	void refreshDoorList();
	const std::vector<void *>& getDoors();
	bool doorListChanged();
	void setScene(std::uint32_t scene);
	std::uint32_t getScene();
	std::array<float, 3> getSceneEntryCoords();

	void setDifficulty(Difficulty value);
	Difficulty getDifficulty();

	void toggleNoclip(bool toggle);
	bool isNoclipOn();

	void spawnPickup(float coords[3], std::uint32_t id, std::uint32_t amount);
	void spawnPickup(std::uint32_t id, std::uint32_t amount);

	InventoryIconData getItemDimensions(ItemId id);
	void setMaxItemAmount(ItemId id, std::uint32_t amount);

	void toggleFastTmp(bool toggle);
	bool isFastTmpEnabled();

	void loadSceneFile(const std::string &sceneName);

	//returns old callback
	void setLoggerCallback(void(__cdecl *callback)(const char*, ...));// -> void(__cdecl*)(const char *, ...);

	void openTypewriter(TypewriterMode mode);
	void openMerchant();
};

class Game::ItemData //Must be 14 bytes
{
	ItemId mId;
	std::uint16_t mAmount; //not ammo
	std::uint16_t mIsValid; //if this is 0, the item will not be in the inventory.
	std::uint16_t mFirePower: 4;
	std::uint16_t mFiringSpeed: 4;
	std::uint16_t mReloadSpeed: 4;
	std::uint16_t mCapacity: 4;
	std::uint16_t mAmmo; //shift to the right 3 times to get actual ammo
	std::uint8_t mPosX; //zero based coordinates of the lower right square occupied by the item. higher bits are Y, lower bits are X. Coordinates increase by 2 FSR.
	std::uint8_t mPosY;
	std::uint8_t mRotation; //0: right; 1: bottom; 2: left; 3: top
	std::uint8_t mInInventory; //1 when the gun is in inventory, 0 when moved outside of it (to discard it)

	ItemData() = delete;
	ItemData(const ItemData&) = delete;
	ItemData(ItemData&&) = delete;
	ItemData& operator=(const ItemData&) = delete;
	ItemData& operator=(ItemData&&) = delete;
public:
	void itemId(ItemId id);
	ItemId itemId() const;

	void amount(std::uint16_t amount);
	std::uint16_t amount() const;

	void valid(bool valid);
	bool valid() const;

	void firingSpeed(std::uint16_t level);
	std::uint16_t firingSpeed() const;

	void firePower(std::uint16_t level);
	std::uint16_t firePower() const;

	void capacity(std::uint16_t level);
	std::uint16_t capacity() const;

	void reloadSpeed(std::uint16_t level);
	std::uint16_t reloadSpeed() const;

	void ammo(std::uint16_t count);
	std::uint16_t ammo() const;

	void posX(std::uint8_t pos);
	std::uint8_t posX() const;

	void posY(std::uint8_t pos);
	std::uint8_t posY() const;

	void rotation(std::uint8_t value);
	std::uint8_t rotation() const;

	void inInventory(bool value);
	std::uint8_t inInventory() const;
};

class Game::WeaponData
{
public:
	static constexpr size_t capacitySlotCount = 7;
private:
	ItemId mId; //Weapon ID
	std::uint8_t mUnknown;
	std::uint8_t mFirepowerIndex;
	std::uint8_t mModel;
	std::uint8_t mAttachmentCount;
	ItemId mAmmoItemId; //-1 on rocket launcher
	std::uint16_t mCapacityValues[capacitySlotCount]; //0x8000 (a.k.a 32768) for infinite ammo

public:

	ItemId id() const;

	void firepowerIndex(std::uint8_t position);
	std::uint8_t firepowerIndex() const;

	void model(std::uint8_t value);
	std::uint8_t model() const;

	void weaponAmmo(ItemId id);
	ItemId weaponAmmo() const;

	void capacity(size_t position, std::uint16_t value);
	std::uint16_t capacity(size_t position) const;
};

class Game::InventoryIconData
{
	enum class ItemType : std::uint8_t { Gun = 1, Ammo, Grenade, Medicine = 6, WeaponAttachment = 9, File, AttacheCase };
	std::uint16_t mUnknown;
	ItemType mItemType;
	std::uint8_t mUnknown2;
	std::uint8_t mStackLimit;
	std::uint8_t mUnknown3;
	std::uint16_t mUnknown4;
public:
	void stackLimit(std::uint8_t value);
	std::uint8_t stackLimit() const;
};

#endif