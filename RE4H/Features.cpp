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

namespace Features
{
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

	enum class GameState : std::uint32_t
	{
		Playing = 0x3,
		ChangingScene = 0x4,
		Pause = 0x106
	};

	struct Entity
	{
		Pointer mVTable;
		char mPadding1[0x4];
		Entity *mNext;
		char mPadding2[0x88];
		Coordinates mCoords;
		char mPadding3[0x4];
		float mRotation;
		char mPadding4[0x54];
		std::uint8_t mMeleeFlag; //4 == meleeing
		std::uint8_t mMovementFlag; //0 == Still | 1 == Walking forward | 2 == Walking backwards | 3 == Running | 4 == Turning | 5 == Quickturning
		std::uint8_t mRunningFlag; //1 == Not running | 3 == Running
		std::uint8_t mAimingFlag; //1 == Aiming
		char mPadding5[0x10];
		Coordinates mCoords2;
		char mPadding6[0x208];
		std::uint16_t mHealth;
		std::uint16_t mHealthLimit;
	}; //highest known offset: 0x79C

	static_assert(offsetof(Features::Entity, Features::Entity::mVTable) == 0x0, "Bad offset");
	static_assert(offsetof(Features::Entity, Features::Entity::mNext) == 0x8, "Bad offset");
	static_assert(offsetof(Features::Entity, Features::Entity::mCoords) == 0x94, "Bad offset");
	static_assert(offsetof(Features::Entity, Features::Entity::mRotation) == 0xA4, "Bad offset");
	static_assert(offsetof(Features::Entity, Features::Entity::mMeleeFlag) == 0xFC, "Bad offset");
	static_assert(offsetof(Features::Entity, Features::Entity::mMovementFlag) == 0xFD, "Bad offset");
	static_assert(offsetof(Features::Entity, Features::Entity::mRunningFlag) == 0xFE, "Bad offset");
	static_assert(offsetof(Features::Entity, Features::Entity::mAimingFlag) == 0xFF, "Bad offset");
	static_assert(offsetof(Features::Entity, Features::Entity::mCoords2) == 0x110, "Bad offset");
	static_assert(offsetof(Features::Entity, Features::Entity::mHealth) == 0x324, "Bad offset");
	static_assert(offsetof(Features::Entity, Features::Entity::mHealthLimit) == 0x326, "Bad offset");

	namespace
	{
		constexpr const char *kDatabaseName = "RE4H.db";
		Pointer gHealthBase; //bio4.exe+806F3C
		Pointer gPlayerBase; //bio4.exe+870FD4
		Pointer gWeaponDataIndex; //bio4.exe+724B10
		Pointer gFirePowerTable; //bio4.exe+800B18
		Pointer gNoclipAddress; //bio4.exe+192E40
		Pointer gDoorData; //bio4.exe+8502C0
		Pointer gDoorList; //bio4.exe+867728. At dereference, then at +0x10, first four bits are door index * 2
		Pointer gDropRandomizerHookLocation; //bio4.exe+1BDF1A
		Pointer gDropRandomizerOriginal = nullptr;
		Pointer gGetModelDataHookLocation; //bio4.exe+3898A6
		Pointer gGetModelDataOriginal = nullptr;
		Pointer gSceAtHookLocation; //bio4.exe+23D004
		Pointer gSceAtOriginal = nullptr;
		Pointer gTmpFireRate; //bio4.exe+70F8BC
		Pointer gLoggerFunction; //bio4.exe+8300
		Pointer gLoggerFunction2; //bio4.exe+CE5F
		Pointer gLinkedList; //bio4.exe+E6E608
		Pointer gTypewriterProcedure; //bio4.exe+563FF0
		Entity **gEntityList; //bio4.exe+7FDB18
		Pointer gEnemyVTable; //bio4.exe+71035C. VTable for enemies that can be meelee'd
		Entity **gPlayerNode; //bio4.exe+857054
		Pointer gUseDoorHookLocation; //bio4.exe+2BB4DF + 1 * 8
		Pointer gOriginalLogger = nullptr, gOriginalLogger2 = nullptr;
		void(__cdecl *gUseDoor)(void*, void*); //first parameter is a pointer to a 312 byte (0x138) structure
		void(__cdecl *gSceAtCreateItemAt)(Coordinates coords, ItemId itemId, int32_t amount, int32_t /*3 for treasures*/, int32_t, int32_t, int32_t);
		void(__cdecl *gGetInventoryModelData)(ItemId id, InventoryIconData *result);
		std::uint32_t(__cdecl *gReadMinimumHeader)(void *sceneHandle, void *unknown);
		void(__cdecl *gOpenMerchant)(std::int32_t, std::int32_t);
		void(__cdecl *gMelee)(void *enemyPointer, void *meleeProcedure);
		void(__cdecl *gMeleeHead)(void *enemyPointer, void*);
		void(__cdecl *gMeleeKnee)(void *enemyPointer, void*);
		void(__cdecl *gMeleeKneeKrauser)(void *enemyPointer, void*);
		void(__cdecl *gMeleeKneeSuplex)(void *enemyPointer, void*);
		std::mutex gStackCapMutex;
		std::vector<void*> gDoors;
		std::map<ItemId, std::uint32_t> gItemStackCap;
		std::function<void(std::uint32_t, const std::vector<void*>&)> gDoorListUpdateCallback;
		const std::map<ItemId, String> gItems = {
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

		ItemId& operator++(ItemId &id)
		{
			if (static_cast<uint16_t>(id) >= static_cast<uint16_t>(ItemId::Last))
				throw std::out_of_range("ItemId increment past ItemId::Last");

			id = static_cast<ItemId>(static_cast<std::uint16_t>(id) + 1);

			return id;
		}
	}

	void ItemData::itemId(ItemId id)
	{
		mId = id;
	}

	ItemId ItemData::itemId() const
	{
		return mId;
	}

	void ItemData::amount(std::uint16_t amount)
	{
		mAmount = amount;
	}

	std::uint16_t ItemData::amount() const
	{
		return mAmount;
	}

	void ItemData::valid(bool valid)
	{
		mIsValid = valid;
	}

	bool ItemData::valid() const
	{
		return mIsValid ? true : false;
	}

	void ItemData::inventory(std::uint8_t inventory)
	{
		mInventory = inventory;
	}

	std::uint8_t ItemData::inventory() const
	{
		return mInventory;
	}

	void ItemData::firingSpeed(std::uint16_t level)
	{
		mFiringSpeed = level;
	}

	std::uint16_t ItemData::firingSpeed() const
	{
		return mFiringSpeed;
	}

	void ItemData::firePower(std::uint16_t level)
	{
		mFirePower = level;
	}

	std::uint16_t ItemData::firePower() const
	{
		return mFirePower;
	}

	void ItemData::capacity(std::uint16_t level)
	{
		mCapacity = level;
	}

	std::uint16_t ItemData::capacity() const
	{
		return mCapacity;
	}

	void ItemData::reloadSpeed(std::uint16_t level)
	{
		mReloadSpeed = level;
	}

	std::uint16_t ItemData::reloadSpeed() const
	{
		return mReloadSpeed;
	}

	void ItemData::ammo(std::uint16_t count)
	{
		mAmmo = count << 3;
	}

	std::uint16_t ItemData::ammo() const
	{
		return mAmmo >> 3;
	}

	void ItemData::posX(std::uint8_t pos)
	{
		mPosX = pos;
	}

	std::uint8_t ItemData::posX() const
	{
		return mPosX;
	}

	void ItemData::posY(std::uint8_t pos)
	{
		mPosY = pos;
	}

	std::uint8_t ItemData::posY() const
	{
		return mPosY;
	}

	void ItemData::rotation(std::uint8_t value)
	{
		mRotation = value;
	}

	std::uint8_t ItemData::rotation() const
	{
		return mRotation;
	}

	void ItemData::inInventory(bool value)
	{
		mInInventory = value;
	}

	std::uint8_t ItemData::inInventory() const
	{
		return mInInventory;
	}

	ItemId WeaponData::id() const
	{
		return mId;
	}

	void WeaponData::firepowerIndex(std::uint8_t position)
	{
		mFirepowerIndex = position;
	}

	std::uint8_t WeaponData::firepowerIndex() const
	{
		return mFirepowerIndex;
	}

	void WeaponData::model(std::uint8_t value)
	{
		mModel = value;
	}

	std::uint8_t WeaponData::model() const
	{
		return mModel;
	}

	void WeaponData::weaponAmmo(ItemId id)
	{
		mAmmoItemId = id;
	}

	ItemId WeaponData::weaponAmmo() const
	{
		return mAmmoItemId;
	}

	void WeaponData::capacity(size_t position, std::uint16_t value)
	{
		mCapacityValues[position] = value;
	}

	std::uint16_t WeaponData::capacity(size_t position) const
	{
		return mCapacityValues[position];
	}

	void InventoryIconData::stackLimit(std::uint8_t value)
	{
		mStackLimit = value;
	}

	std::uint8_t InventoryIconData::stackLimit() const
	{
		return mStackLimit;
	}

	void InventoryIconData::itemType(ItemType type)
	{
		mItemType = type;
	}

	InventoryIconData::ItemType InventoryIconData::itemType() const
	{
		return mItemType;
	}

	void __cdecl myGetInventoryModelData(ItemId id, InventoryIconData *result)
	{
		if (id != ItemId::Invalid)
		{
			std::unique_lock<std::mutex> lck(gStackCapMutex);
			auto amountIt = gItemStackCap.find(id);

			gGetInventoryModelData(id, result);

			if (amountIt != gItemStackCap.end())
				result->stackLimit(amountIt->second);
		}
	}

	int __cdecl myDropRandomizer(std::uint32_t enemyId, ItemId *outItemId, std::uint32_t *outItemCount, void *unknownPassZero)
	{
		bool result = false;
		std::vector<ItemId> candidates = { ItemId::TreasureBoxS, ItemId::FlashGrenade, ItemId::IncendiaryGrenade, ItemId::HandGrenade };
		static std::random_device hardwareEngine;
		static std::mt19937 softwareEngine(hardwareEngine.entropy() ? hardwareEngine() : static_cast<std::mt19937::result_type>(time(nullptr)));
		//static std::uniform_int_distribution<std::remove_reference<decltype(candidates)>::type::size_type> randomizer(/*ItemIds::MagnumAmmo*/0, /*ItemIds::Mission5TreasureMap*/300);
		InventoryIconData icon;

		if (GetHealth() <= GetHealthLimit() / 2)
		{
			candidates.push_back(ItemId::GreenHerb);
			candidates.push_back(ItemId::FirstAidSpray);
		}

		for (ItemData *item = BeginInventory(), *end = EndInventory(); item != end; ++item)
		{
			if (item->valid())
			{
				auto &ammoIds = GetAmmoItemIds();
				WeaponData *gun = GetWeaponDataPtr(item->itemId());

				if (gun && std::find(ammoIds.cbegin(), ammoIds.cend(), gun->weaponAmmo()) != ammoIds.end())
				{
					candidates.push_back(gun->weaponAmmo());
					candidates.push_back(gun->weaponAmmo());
					candidates.push_back(gun->weaponAmmo());
					candidates.push_back(gun->weaponAmmo());
				}
			}
		}

		std::uniform_int_distribution<size_t> candidateDistribution(0, candidates.size() - 1);

		if (hardwareEngine.entropy())
			*outItemId = candidates[candidateDistribution(hardwareEngine) % candidates.size()];
		else
			*outItemId = candidates[candidateDistribution(softwareEngine) % candidates.size()];
		gGetInventoryModelData(*outItemId, &icon);
		std::uniform_int_distribution<std::uint32_t> itemCountDistribution(1, icon.stackLimit());
		*outItemCount = itemCountDistribution(softwareEngine);

		result = true;

		return result;
	}

	std::uint32_t __cdecl sceAtHook(void *arg1, void *arg2)
	{
		std::uint32_t result = reinterpret_cast<decltype(sceAtHook)*>(gSceAtOriginal)(arg1, arg2);

		RefreshDoorList();
		if (gDoorListUpdateCallback)
			gDoorListUpdateCallback(getValue<std::uint32_t>(gHealthBase + HealthBaseOffsets::Scene), gDoors);

		return result;
	}

	void __cdecl useDoorHook(void *arg, void *arg2)
	{
		sqlite3 *database = nullptr;

		gUseDoor(arg, arg2);

		if ((sqlite3_open(kDatabaseName, &database) & 0xFF) == SQLITE_OK)
		{
			std::string_view sql("INSERT INTO scene_entry(scene_id, x, y, z, rotation) VALUES (?, ?, ?, ?, ?)");
			sqlite3_stmt *statement = nullptr;

			if ((sqlite3_prepare_v2(database, sql.data(), static_cast<int>(sql.size() + 1), &statement, nullptr) & 0xFF) == SQLITE_OK)
			{
				sqlite3_bind_int(statement, 1, getValue<int>(gHealthBase + HealthBaseOffsets::Scene));
				sqlite3_bind_double(statement, 2, getValue<float>(gHealthBase + HealthBaseOffsets::SceneEntryX));
				sqlite3_bind_double(statement, 3, getValue<float>(gHealthBase + HealthBaseOffsets::SceneEntryY));
				sqlite3_bind_double(statement, 4, getValue<float>(gHealthBase + HealthBaseOffsets::SceneEntryZ));
				sqlite3_bind_double(statement, 5, getValue<float>(gHealthBase + HealthBaseOffsets::Rotation));
				sqlite3_step(statement);
				sqlite3_finalize(statement);
			}
		}

		sqlite3_close_v2(database);
	}
	
	bool operator==(Character lhs, std::uint8_t rhs)
	{
		return lhs == Character::Leon && rhs == 0
			|| lhs == Character::Ashley && rhs == 1
			|| lhs == Character::Ada && rhs == 2
			|| lhs == Character::HUNK && rhs == 3
			|| lhs == Character::Krauser && rhs == 4
			|| lhs == Character::Wesker && rhs == 5;
	}

	bool operator==(std::uint8_t lhs, Character rhs)
	{
		return rhs == lhs;
	}

	bool operator!=(Character lhs, std::uint8_t rhs)
	{
		return !(lhs == rhs);
	}

	bool operator!=(std::uint8_t lhs, Character rhs)
	{
		return !(lhs == rhs);
	}

	Character GetCharacterFromId(std::uint8_t id)
	{
		Character result;

		switch (id)
		{
			case 0:
				result = Character::Leon;
				break;

			case 1:
				result = Character::Ashley;
				break;

			case 2:
				result = Character::Ada;
				break;

			case 3:
				result = Character::HUNK;
				break;

			case 4:
				result = Character::Krauser;
				break;

			case 5:
				result = Character::Wesker;
				break;

			default:
				throw std::runtime_error("Invalid character id");
		}

		return result;
	}

	std::uint8_t GetCharacterId(Character character)
	{
		return static_cast<std::uint8_t>(character);
	}

	bool Initialize()
	{
		sqlite3 *database = nullptr;

		gHealthBase = patternScan("A1 ????????  83 C0 60  6A 10  50  E8");
		gPlayerBase = patternScan("B9 ????????  E8 ????????  8B 35 ????????  81");
		gWeaponDataIndex = patternScan("B9 ????????  8D A4 24 00000000  66 3B 31  74 10  40");
		gFirePowerTable = patternScan("D9 04 8D ????????  D9 5D 08");
		gNoclipAddress = patternScan("55 8B EC 53 8B 5D 08 56 8B B3 F4 00 00 00 57 8B 7D 0C 57");
		gDoorData = patternScan("B9 ????????  6A 0B  75 ??  6A 3A");
		gDoorList = patternScan("A1 ????????  53  05 8C000000  56  C6 45 CB 00");
		gDropRandomizerHookLocation = patternScan("E8 ????????  83 C4 10  83 F8 01  75 ??  8B 45 08  8B 4D FC");
		gGetModelDataHookLocation = patternScan("E8 ????????  83 C4 08  80 7D 8A 01  74");
		gSceAtHookLocation = patternScan("E8 ????????  8B 0D ????????  A1 ????????  8D 14 09");
		gTmpFireRate = patternScan("D9 05 ????????  D9 45 D4  D8D1  DFE0  DDD9  F6 C4 41");
		gLoggerFunction = patternScan("E8 ????????  83 C4 08  E8 ????????  53  0FB7 5F 10");
		gLoggerFunction2 = patternScan("50  68 ????????  6A 00  6A 00  E8 ????????  83 C4 10  33 C0  8B E5  5D  C3");
		gLinkedList = patternScan("BB ????????  E8 ????????  89 45 FC  EB 03  8B 45 FC");
		gTypewriterProcedure = patternScan("55  8B EC  A1 ????????  81 88 28500000 00080000");
		gEntityList = reinterpret_cast<decltype(gEntityList)>(patternScan("8B 35 ????????  85 F6  74 43  8B C6"));
		gEnemyVTable = patternScan("C7 06 ????????  8B C6  5E  5D  C2 0400  33 C0  5E  5D  C2 0400  CCCCCCCCCCCCCCCCCCCC 55  8B EC  56  8B 75 08  85 F6  74 0D  8B CE  E8 ????????  C7 06 ????????  5E  5D  C3  CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC  C7 05 ???????? ????????  C7 05 ???????? ????????  C3");
		gUseDoorHookLocation = patternScan("8B 04 DD ????????  8B 55 BC");
		gUseDoor = reinterpret_cast<decltype(gUseDoor)>(patternScan("55  8B EC  A1 ????????  53  33 DB  F7 40 54 ????????"));
		gSceAtCreateItemAt = reinterpret_cast<decltype(gSceAtCreateItemAt)>(patternScan("55  8B EC  83 EC 0C  57  6A 0D"));
		gGetInventoryModelData = reinterpret_cast<decltype(gGetInventoryModelData)>(patternScan("55  8B EC  0FB7 45 ??  3D 0F010000"));
		gReadMinimumHeader = reinterpret_cast<decltype(gReadMinimumHeader)>(patternScan("55  8B EC  53  8B 1D ????????  56  68 ????????"));
		gOpenMerchant = reinterpret_cast<decltype(gOpenMerchant)>(patternScan("55  8B EC  A1 ????????  B9 00000004"));
		gMeleeHead = reinterpret_cast<decltype(gMeleeHead)>(patternScan("55  8B EC  A1 ????????  0FB6 80 ????????  83 E8 03"));
		gMeleeKnee = reinterpret_cast<decltype(gMeleeKnee)>(patternScan("55  8B EC  A1 ????????  80 B8 C84F0000 04"));
		gMeleeKneeKrauser = reinterpret_cast<decltype(gMeleeKneeKrauser)>(patternScan("55  8B EC  8B 45 08  80 B8 2C030000 00"));
		gMeleeKneeSuplex = reinterpret_cast<decltype(gMeleeKneeSuplex)>(patternScan("55  8B EC  8B 45 08  80 B8 2C030000 00"));
		gMelee = reinterpret_cast<decltype(gMelee)>(patternScan("55  8B EC  56  8B 35 ????????  8B CE  E8 ????????  8B 45 0C"));

		if (!(gHealthBase && gPlayerBase && gWeaponDataIndex && gFirePowerTable && gNoclipAddress && gDoorData
			  && gDoorList && gDropRandomizerHookLocation && gGetModelDataHookLocation && gSceAtHookLocation && gTmpFireRate && gLoggerFunction
			  && gLoggerFunction2 && gLinkedList && gTypewriterProcedure && gEntityList && gEnemyVTable && gUseDoorHookLocation
			  && gUseDoor && gSceAtCreateItemAt && gGetInventoryModelData && gReadMinimumHeader && gOpenMerchant && gMeleeHead
			  && gMeleeKnee && gMeleeKneeKrauser && gMeleeKneeSuplex && gMelee))
			return false;

		if ((sqlite3_open(kDatabaseName, &database) & 0xFF) == SQLITE_OK)
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

					gItemStackCap[static_cast<ItemId>(itemId)] = limit;
				}

				sqlite3_finalize(statement);
			}
		}

		sqlite3_close_v2(database);

		gHealthBase = pointerPath(gHealthBase, 0x1, 0x0);
		gPlayerBase = getValue<Pointer>(gPlayerBase + 1);
		gWeaponDataIndex = getValue<Pointer>(gWeaponDataIndex + 1);
		gFirePowerTable = getValue<Pointer>(gFirePowerTable + 3);
		gDoorData = getValue<Pointer>(gDoorData + 1);
		gDoorList = getValue<Pointer>(gDoorList + 1);
		gGetModelDataHookLocation = follow(gGetModelDataHookLocation);
		gDropRandomizerHookLocation = follow(gDropRandomizerHookLocation);
		gSceAtHookLocation = follow(gSceAtHookLocation);
		gLoggerFunction = follow(gLoggerFunction);
		gLoggerFunction2 = follow(gLoggerFunction2 + 10);
		gTmpFireRate = getValue<Pointer>(gTmpFireRate + 2);
		gLinkedList = getValue<Pointer>(gLinkedList + 1);
		gPlayerNode = getValue<Entity**>(reinterpret_cast<Pointer>(gEntityList) + 17);
		gEntityList = getValue<Entity**>(reinterpret_cast<Pointer>(gEntityList) + 2);
		gEnemyVTable = getValue<Pointer>(gEnemyVTable + 2);
		gUseDoorHookLocation = getValue<Pointer>(gUseDoorHookLocation + 3) + 1 * 8;

		//Install hooks
		gGetModelDataOriginal = replaceFunction(gGetModelDataHookLocation, myGetInventoryModelData);
		gDropRandomizerOriginal = replaceFunction(gDropRandomizerHookLocation, myDropRandomizer);
		gSceAtOriginal = replaceFunction(gSceAtHookLocation, sceAtHook);
		gOriginalLogger = replaceFunction(gLoggerFunction, gLoggerFunction);
		gOriginalLogger2 = replaceFunction(gLoggerFunction2, gLoggerFunction2);
		setValue(gUseDoorHookLocation, useDoorHook);

		#ifndef NDEBUG
		using std::cout;
		using std::endl;
		cout << "Health Base: " << (void*)gHealthBase << endl;
		cout << "Player Base: " << (void*)gPlayerBase << endl;
		cout << "Weapon Data Index: " << (void*)gWeaponDataIndex << endl;
		cout << "Fire power table: " << (void*)gFirePowerTable << endl;
		cout << "Noclip code: " << (void*)gNoclipAddress << endl;
		cout << "Door data: " << (void*)gDoorData << endl;
		cout << "Door List: " << (void *)gDoorList << endl;
		cout << "Set Scene: " << (void*)gUseDoor << endl;
		cout << "SceCreateItemAt: " << (void*)gSceAtCreateItemAt << endl;
		cout << "Logger Function Jump: " << (void*)gLoggerFunction << endl;
		#endif

		return true;
	}

	void Terminate()
	{
		replaceFunction(gGetModelDataHookLocation, gGetModelDataOriginal);
		replaceFunction(gDropRandomizerHookLocation, gDropRandomizerOriginal);
		replaceFunction(gSceAtHookLocation, gSceAtOriginal);
		replaceFunction(gLoggerFunction, gOriginalLogger);
		replaceFunction(gLoggerFunction2, gOriginalLogger2);
		setValue(gUseDoorHookLocation, gUseDoor);
	}

	void SetHealth(std::uint16_t health)
	{
		setValue(gHealthBase + HealthBaseOffsets::Health, health);
	}

	std::uint16_t GetHealth()
	{
		return getValue<std::uint16_t>(gHealthBase + HealthBaseOffsets::Health);
	}

	void SetHealthLimit(std::uint16_t limit)
	{
		setValue(gHealthBase + HealthBaseOffsets::MaxHealth, limit);
	}

	std::uint16_t GetHealthLimit()
	{
		return getValue<std::uint16_t>(gHealthBase + HealthBaseOffsets::MaxHealth);
	}

	String GetItemName(ItemId id)
	{
		return gItems.at(id);
	}

	ItemData* BeginInventory()
	{
		return getValue<ItemData*>(gPlayerBase + PlayerBaseOffsets::Inventory);
	}

	ItemData* EndInventory()
	{
		return getValue<ItemData*>(gPlayerBase + PlayerBaseOffsets::Inventory) + getValue<std::uint32_t>(gPlayerBase + PlayerBaseOffsets::InventorySize);
	}

	ItemData* AddItem()
	{
		std::uint32_t invSize = getValue<std::uint32_t>(gPlayerBase + PlayerBaseOffsets::InventorySize);
		ItemData *result = getValue<ItemData*>(gPlayerBase + PlayerBaseOffsets::Inventory), *end = result + invSize;

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

	const std::vector<String>& GetItemNames()
	{
		static std::vector<String> itemNames;

		if (itemNames.empty())
		{
			itemNames.reserve(gItems.size());
			for (const auto &item : gItems)
				itemNames.push_back(item.second);
		}

		return itemNames;
	}

	bool IsAshleyPresent()
	{
		return getValue<std::uint32_t>(gHealthBase + HealthBaseOffsets::AshleyPresent) & 0x04000000;
	}

	void ToggleAshley(bool toggle)
	{
		if (Entity *playerEntity = *gPlayerNode)
		{
			setValue<std::uint32_t>(gHealthBase + HealthBaseOffsets::AshleyPresent, toggle ? 0x04000000 : 0);
			setValue<Coordinates>(gHealthBase + HealthBaseOffsets::SceneEntryX, playerEntity->mCoords);
			setValue<float>(gHealthBase + HealthBaseOffsets::Rotation, playerEntity->mRotation);
			setValue<GameState>(gHealthBase + HealthBaseOffsets::Status, GameState::ChangingScene);
		}
	}

	void SetCharacter(Character id)
	{
		setValue<Character>(gHealthBase + HealthBaseOffsets::Character, id);
	}

	Character GetCharacter()
	{
		return getValue<Character>(gHealthBase + HealthBaseOffsets::Character);
	}

	void SetCostume(std::uint8_t id)
	{
		switch (GetCharacter())
		{
			case Character::Leon:
				if (id <= 4)
					setValue(gHealthBase + HealthBaseOffsets::Costume, id);
				break;
			case Character::Ashley:
				if (id <= 2)
					setValue<std::uint8_t>(gHealthBase + HealthBaseOffsets::Costume, id);
				break;
			case Character::Ada:
				if (id <= 2)
					setValue<std::uint8_t>(gHealthBase + HealthBaseOffsets::Costume, id != 2 ? id : 3);
				break;
			case Character::HUNK:
			case Character::Krauser:
			case Character::Wesker:
				if (!id)
					setValue<std::uint8_t>(gHealthBase + HealthBaseOffsets::Costume, id);
				break;
		}
	}

	std::uint8_t GetCostume()
	{
		auto costume = getValue<std::uint8_t>(gHealthBase + HealthBaseOffsets::Costume);

		if (GetCharacter() == Character::Ada && costume == 3)
			costume = 2;

		return costume;
	}

	void FixCostume()
	{
		auto costume = GetCostume();

		switch (GetCharacter())
		{
			case Character::Leon:
				if (costume > 4)
					SetCostume(0);
				break;

			case Character::Ashley:
				if (costume > 2)
					SetCostume(0);
				break;

			case Character::Ada:
				if (costume > 2)
					SetCostume(2);
				break;

			case Character::HUNK:
			case Character::Krauser:
			case Character::Wesker:
				if (costume)
					SetCostume(0);
				break;
		}
	}

	const std::vector<StringView> GetCharacterCostumeNames(Character id)
	{
		std::vector<StringView> result;

		switch (GetCharacter())
		{
			case Character::Leon:
				result = { TEXT("Normal with Jacket"), TEXT("Normal Without Jacket"), TEXT("Normal With Vest"), TEXT("R.P.D"), TEXT("Mafia") };
				break;

			case Character::Ashley:
				result = { TEXT("Normal"), TEXT("Pop star"), TEXT("Knight") };
				break;

			case Character::Ada:
				result = { TEXT("The Mercenaries"), TEXT("Assignment Ada"), TEXT("Separate Ways") };
				break;

			case Character::HUNK:
			case Character::Krauser:
			case Character::Wesker:
				result = { TEXT("Normal") };
				break;
		}

		return result;
	}

	WeaponData* GetWeaponDataPtr(ItemId id)
	{
		WeaponData *result = nullptr, *iter = (WeaponData*)gWeaponDataIndex;

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

	void SetWeaponDataPtr(WeaponData *target, const WeaponData &source, const float(&newFirepower)[7])
	{
		DWORD oldProtect;
		VirtualProtect(target, sizeof(WeaponData), PAGE_READWRITE, &oldProtect);

		SetFirepowerTableEntry(target->firepowerIndex(), newFirepower);
		for (size_t i = 0; i < WeaponData::capacitySlotCount; ++i)
			target->capacity(i, source.capacity(i));
		target->weaponAmmo(source.weaponAmmo());
		target->model(source.model());

		VirtualProtect(target, sizeof(WeaponData), oldProtect, &oldProtect);
	}

	bool IsWeapon(ItemId id)
	{
		return GetWeaponDataPtr(id) ? true : false;
	}

	float* GetFirepowerTableEntry(std::uint8_t i)
	{
		return reinterpret_cast<float*>(gFirePowerTable + i * 7 * sizeof(float));
	}

	void SetFirepowerTableEntry(std::uint8_t i, const float(&newValues)[7])
	{
		setValue(gFirePowerTable + i * 7 * sizeof(float), newValues);
	}

	const std::vector<ItemId> GetAmmoItemIds()
	{
		std::vector<ItemId> result;

		for (ItemId id = ItemId::First; id != ItemId::Last; ++id)
		{
			InventoryIconData data = {};

			gGetInventoryModelData(id, &data);

			if (data.itemType() == InventoryIconData::ItemType::AMMO)
				result.push_back(id);
		}

		return result;
	}

	void SetMoney(std::uint32_t value)
	{
		setValue<std::uint32_t>(gHealthBase + HealthBaseOffsets::Money, value);
	}

	std::uint32_t GetMoney()
	{
		return getValue<std::uint32_t>(gHealthBase + HealthBaseOffsets::Money);
	}

	void UseDoor(void *doorData)
	{
		useDoorHook(doorData, nullptr);
	}

	Pointer GetFirstValidDoor()
	{
		Pointer result = nullptr;

		if (result = getValue<Pointer>(gDoorList))
		{
			result = getValue<Pointer>(result + 4);

			while (!((std::uint32_t)result & 1))
			{
				if (result && getValue<std::uint8_t>(result + DoorListOffsets::Type) == 1)
					return result;
				if (result)
					result = getValue<Pointer>(result);
				else
					break;
			}

			return nullptr;
		}

		return result;
	}

	void RefreshDoorList()
	{
		Pointer door = GetFirstValidDoor();

		gDoors.clear();

		if (door)
		{
			while (!((std::uint32_t)door & 1))
			{
				if (getValue<std::uint8_t>(door + 0x35) == 1) //if it's a door
					gDoors.push_back(door);

				door = getValue<Pointer>(door);
			}
		}
	}

	const std::vector<void*> GetDoors()
	{
		return gDoors;
	}

	void SetScene(std::uint32_t scene)
	{
		if (getValue<GameState>(gHealthBase + HealthBaseOffsets::Status) == GameState::Playing)
		{
			float entry[4] = {};
			sqlite3 *database = nullptr;

			setValue<std::uint32_t>(gHealthBase + HealthBaseOffsets::Scene, scene);

			if ((sqlite3_open(kDatabaseName, &database) & 0xFF) == SQLITE_OK)
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
			setValue(gHealthBase + HealthBaseOffsets::SceneEntryX, entry);
			setValue(gHealthBase + HealthBaseOffsets::Status, GameState::ChangingScene);
		}
	}

	std::uint32_t GetScene()
	{
		return getValue<std::uint32_t>(gHealthBase + HealthBaseOffsets::Scene);
	}

	Coordinates GetSceneEntryCoords()
	{
		return getValue<Coordinates>(gHealthBase + HealthBaseOffsets::SceneEntryX);
	}

	void SetDifficulty(Difficulty value)
	{
		setValue<Difficulty>(gHealthBase + HealthBaseOffsets::Difficulty, value);
	}

	Difficulty GetDifficulty()
	{
		return getValue<Difficulty>(gHealthBase + HealthBaseOffsets::Difficulty);
	}

	void ToggleNoclip(bool toggle)
	{
		DWORD oldProtect;

		if (VirtualProtect(gNoclipAddress, 1, PAGE_EXECUTE_READWRITE, &oldProtect))
		{
			setValue<std::uint8_t>(gNoclipAddress, toggle ? 0xC3 /*ret*/ : 0x55 /*push ebp*/);
			VirtualProtect(gNoclipAddress, 1, oldProtect, &oldProtect);
		}
	}

	bool IsNoclipOn()
	{
		return getValue<std::uint8_t>(gNoclipAddress) == 0xC3 /*ret*/ ? true : false;
	}

	void SpawnPickup(const Coordinates &coords, ItemId id, std::uint32_t amount)
	{
		gSceAtCreateItemAt(coords, id, amount, 3, -1, 0, -1);
	}

	void SpawnPickup(ItemId id, std::uint32_t amount)
	{
		Coordinates coords{ getValue<float>(gHealthBase + HealthBaseOffsets::PlayerX), getValue<float>(gHealthBase + HealthBaseOffsets::PlayerY), getValue<float>(gHealthBase + HealthBaseOffsets::PlayerZ) };
		SpawnPickup(coords, id, amount);
	}

	InventoryIconData GetItemDimensions(ItemId id)
	{
		InventoryIconData result = {};
		myGetInventoryModelData(id, &result);
		return result;
	}

	void SetMaxItemAmount(ItemId id, std::uint32_t amount)
	{
		std::unique_lock<std::mutex> lck(gStackCapMutex);
		std::string_view sql("REPLACE INTO stack_limit(item_id, max_amount) VALUES (?, ?)");
		sqlite3 *database = nullptr;

		gItemStackCap[id] = amount;
		lck.unlock();

		if ((sqlite3_open(kDatabaseName, &database) & 0xFF) == SQLITE_OK)
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

	void ToggleMaxItemAmountHook(bool toggle)
	{
		if (toggle)
			replaceFunction(gGetModelDataHookLocation, myGetInventoryModelData);
		else
			replaceFunction(gGetModelDataHookLocation, gGetModelDataOriginal);
	}

	bool IsMaxItemHookEnabled()
	{
		return follow(gGetModelDataHookLocation) != gGetModelDataOriginal;
	}

	void ToggleFastTmp(bool toggle)
	{
		DWORD oldProtect;

		if (VirtualProtect(gTmpFireRate, sizeof(float), PAGE_READWRITE, &oldProtect))
		{
			*(float *)gTmpFireRate = toggle ? 1.5f : 3.0f;
			VirtualProtect(gTmpFireRate, sizeof(float), oldProtect, &oldProtect);
		}
	}

	bool IsFastTmpEnabled()
	{
		return *(float*)gTmpFireRate == 1.5f;
	}

	void LoadSceneFile(const std::string &sceneName)
	{
		HANDLE sceneHandle = CreateFileA(sceneName.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (sceneHandle != INVALID_HANDLE_VALUE)
		{
			gReadMinimumHeader(sceneHandle, nullptr);
			CloseHandle(sceneHandle);
		}
	}

	void SetLoggerCallback(void(__cdecl *callback)(const char*, const char*, const char*, ...))
	{
		replaceFunction(gLoggerFunction, callback);
		replaceFunction(gLoggerFunction2, callback);
	}

	void OpenTypewriter(TypewriterMode mode)
	{
		Pointer node = getValue<Pointer>(gLinkedList + 0x34);
		node = getValue<Pointer>(node + 0x14);
		node += 0x18;

		setValue<TypewriterMode>(node + 0x14, mode);
		setValue<Pointer>(node + 0x33C, gTypewriterProcedure);
		setValue<std::uint8_t>(node + 4, 1);
	}

	void OpenMerchant()
	{
		gOpenMerchant(0x10, 0);
	}

	void Melee(MeleeType type)
	{
		if (Entity *playerEntity = *gPlayerNode)
		{
			auto character = GetCharacter();

			if (character == Character::Krauser) //Game crashes when doing this with Krauser for some reason.
				return;

			for (Entity *node = *gEntityList; node; node = node->mNext)
			{
				if (node->mVTable == gEnemyVTable)
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
							gMeleeHead(node, nullptr);
							break;
						case MeleeType::KNEE:
							switch (character)
							{
								case Character::Krauser:
									gMeleeKneeKrauser(node, nullptr);
									break;
								case Character::Leon:
									if (node->mHealth)
										freezeRotation = false;
									else
										continue;
									gMeleeKneeSuplex(node, nullptr);
									break;
								default:
									gMeleeKnee(node, nullptr);
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

	void SetPlayerCoordinates(const Coordinates &coordinates)
	{
		if (Entity *playerEntity = *gPlayerNode)
			playerEntity->mCoords = coordinates;
	}

	std::optional<Coordinates> GetPlayerCoordinates()
	{
		std::optional<Coordinates> result;

		if (Entity *playerEntity = *gPlayerNode)
			result = playerEntity->mCoords;

		return result;
	}

	std::vector<std::wstring> GetSceneFileNames()
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

	void SetDoorListUpdateCallback(decltype(gDoorListUpdateCallback) callback)
	{
		gDoorListUpdateCallback = callback;
	}

	void EasyDrops(bool toggle)
	{
		if (toggle)
			replaceFunction(gDropRandomizerHookLocation, myDropRandomizer);
		else
			replaceFunction(gDropRandomizerHookLocation, gDropRandomizerOriginal);
	}

	bool EasyDrops()
	{
		return follow(gDropRandomizerHookLocation) != gDropRandomizerOriginal;
	}
}