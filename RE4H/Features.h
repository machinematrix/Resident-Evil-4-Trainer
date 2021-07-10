#ifndef __GAME_H__
#define __GAME_H__
#include "Memory.h"
#include <vector>
#include <array>
#include <optional>
#include <functional>
#include <mutex>
#include <map>
#include <string_view>

#ifdef UNICODE
using String = std::basic_string<wchar_t>;
using StringView = std::basic_string_view<wchar_t>;
#else
using String = std::basic_string<char>;
using StringView = std::basic_string_view<char>;
#endif

namespace Features
{
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
		Invalid = 0xFFFF,
		First = MagnumAmmo,
		Last = Mission5TreasureMap
	};
	enum class Character : std::uint8_t
	{
		Leon,
		Ashley,
		Ada,
		HUNK,
		Krauser,
		Wesker
	};
	enum class MeleeType { HEAD, KNEE };
	enum class Difficulty : std::uint32_t { AMATEUR, EASY = 3, NORMAL = 5, PROFESSIONAL = 6 };
	enum class TypewriterMode : std::uint32_t { LOAD, SAVE };

	struct Coordinates
	{
		float mX;
		float mY;
		float mZ;
	};

	class ItemData //Must be 14 bytes
	{
		ItemId mId;
		std::uint16_t mAmount; //not ammo
		bool mIsValid; //if this is 0, the item will not be in the inventory.
		std::uint8_t mInventory; //0: Leon's inventory | 1: Ashley's inventory
		std::uint16_t mFirePower : 4;
		std::uint16_t mFiringSpeed : 4;
		std::uint16_t mReloadSpeed : 4;
		std::uint16_t mCapacity : 4;
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
		void inventory(std::uint8_t inventory);
		std::uint8_t inventory() const;
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

	class WeaponData
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
		std::uint16_t mCapacityValues[7]; //0x8000 (i.e 32768) for infinite ammo
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

	class InventoryIconData
	{
	public:
		enum class ItemType : std::uint8_t { GUN = 1, AMMO, GRENADE, MEDICINE = 6, WEAPON_ATTACHMENT = 9, FILE, ATTACH_CASE };
	private:
		std::uint16_t mUnknown;
		ItemType mItemType;
		std::uint8_t mUnknown2;
		std::uint8_t mStackLimit;
		std::uint8_t mUnknown3;
		std::uint16_t mUnknown4;
	public:
		void stackLimit(std::uint8_t value);
		std::uint8_t stackLimit() const;
		void itemType(ItemType);
		ItemType itemType() const;
	};

	bool operator==(Character lhs, std::uint8_t rhs);
	bool operator==(std::uint8_t lhs, Character rhs);
	bool operator!=(Character lhs, std::uint8_t rhs);
	bool operator!=(std::uint8_t lhs, Character rhs);
	Character GetCharacterFromId(std::uint8_t id);
	std::uint8_t GetCharacterId(Character character);

	bool Initialize();
	void Terminate();

	void SetHealth(std::uint16_t health);
	std::uint16_t GetHealth();
	void SetHealthLimit(std::uint16_t limit);
	std::uint16_t GetHealthLimit();

	String GetItemName(ItemId id); //throws out_of_range
	ItemData* BeginInventory();
	ItemData* EndInventory();
	ItemData* AddItem();
	const std::vector<String>& GetItemNames();

	bool IsAshleyPresent();
	void ToggleAshley(bool toggle);

	//0: Leon | 1: Ashley | 2: Ada | 3: HUNK | 4: Krauser | 5: Wesker
	void SetCharacter(Character id);
	Character GetCharacter();
	void SetCostume(std::uint8_t id);
	std::uint8_t GetCostume();
	void FixCostume();
	const std::vector<StringView> GetCharacterCostumeNames(Character id);

	WeaponData* GetWeaponDataPtr(ItemId id);
	void SetWeaponDataPtr(WeaponData *target, const WeaponData &source, const float(&newFirepower)[7]);
	bool IsWeapon(ItemId id);
	float* GetFirepowerTableEntry(std::uint8_t i);
	void SetFirepowerTableEntry(std::uint8_t i, const float(&newValues)[7]);
	const std::vector<ItemId> GetAmmoItemIds();

	void SetMoney(std::uint32_t value);
	std::uint32_t GetMoney();

	void UseDoor(void *doorData);
	void RefreshDoorList();
	const std::vector<void *> GetDoors();
	void SetScene(std::uint32_t scene);
	std::uint32_t GetScene();
	Coordinates GetSceneEntryCoords();

	void SetDifficulty(Difficulty value);
	Difficulty GetDifficulty();

	void ToggleClipping(bool toggle);
	bool IsClippingDisabled();

	void SpawnPickup(const Coordinates &coords, ItemId id, std::uint32_t amount);
	void SpawnPickup(ItemId id, std::uint32_t amount);

	InventoryIconData GetItemDimensions(ItemId id);
	void SetMaxItemAmount(ItemId id, std::uint32_t amount);
	void ToggleMaxItemAmountHook(bool toggle);
	bool IsMaxItemHookEnabled();

	void ToggleFastTmp(bool toggle);
	bool IsFastTmpEnabled();

	void LoadSceneFile(const std::string &sceneName);

	void SetLoggerCallback(void(__cdecl *callback)(const char*, const char*, const char*, ...));// -> void(__cdecl*)(const char *, ...);

	void OpenTypewriter(TypewriterMode mode);
	void OpenMerchant();

	void Melee(MeleeType);
	void SetPlayerCoordinates(const Coordinates &coordinates);
	std::optional<Coordinates> GetPlayerCoordinates();

	std::vector<std::wstring> GetSceneFileNames();
	void SetDoorListUpdateCallback(std::function<void(std::uint32_t, const std::vector<void*>&)>);

	void EasyDrops(bool toggle);
	bool EasyDrops();

	//This is to avoid crashes with other characters in the main game.
	void SkipRadioCutscenes(bool skip);
	bool IsRadioSkipEnabled();
}

#endif