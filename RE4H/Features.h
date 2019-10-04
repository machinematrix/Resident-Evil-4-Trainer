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

class Game
{
public:
	struct InventoryIconData
	{
		std::uint16_t unknown;
		std::uint8_t itemType; //1 for guns, 2 for ammo, 3 for grenades, 6 for medicine
		std::uint8_t unknown4;
		std::uint8_t stackLimit;
		std::uint8_t unknown5;
		std::uint16_t unknown2;
	};
private:
	friend void __cdecl myGetInventoryModelData(std::uint32_t, Game::InventoryIconData*);
	friend int __cdecl myDropRandomizer(std::uint32_t, std::uint32_t *, std::uint32_t *, Game *);

	static const Bimap<std::uint16_t, String> items;
	std::mutex doorVectorMutex;
	bool sceneChanged = false;
	Pointer healthBase;
	Pointer playerBase;
	Pointer weaponDataIndex;
	Pointer firePowerTable;
	Pointer noclipAddress;
	Pointer doorData;
	Pointer doorList; //at deference, then at +0x10, first four bits are door index * 2
	Pointer refreshDoorsHookLocation;
	Pointer dropRandomizerHookLocation;
	Pointer dropRandomizerOriginal = nullptr;
	Pointer getModelDataHookLocation;
	Pointer getModelDataOriginal = nullptr;
	Pointer tmpFireRate;
	Pointer loggerFunction;
	Pointer linkedList; //+0x34 next node, then do +0x14, that's second argument to SetPanel
	Pointer typewriterProc;
	//const char *srcSchedulerCpp;

	std::int32_t originalLoggerCallbackOffset;
	std::vector<void*> doors;
	std::map<std::uint16_t, std::uint32_t> itemMaxAmount;
	
	void(__cdecl *setScenePtr)(void*); //first parameter is a pointer to a 312 byte (0x138) structure
	void(__cdecl *sceAtCreateItemAt)(float coords[3], uint32_t itemId, uint32_t amount, uint32_t /*3 for treasures*/, uint32_t, uint32_t, uint32_t);
	void(__cdecl *getInventoryModelData)(std::uint32_t id, InventoryIconData *result);
	void(__stdcall *SaveProcedure)(void *saveStuff);
	std::uint32_t(__cdecl *readMinimumHeader)(void *sceneHandle, void *unkwnown);
	void(__cdecl *SetPanel)(void *node, void *functionAddress, std::uint32_t param, void *memoryPool, std::uint32_t flags, std::uint32_t flags2, std::uint32_t one, const char *string, std::uint32_t xAF);

	template <typename T> T getValue(Pointer address) const;
	template <typename T> void setValue(Pointer address, T value) const;
	template <typename T, size_t sz> void setValue(Pointer address, const T value[sz]) const;
	template <typename T> Pointer pointerPath(Pointer baseAddress, const T& offset);
	template <typename T, typename ...Args> Pointer pointerPath(Pointer baseAddress, const T &offset, const Args& ...offsets);
	Pointer pointerPath(Pointer baseAddress, const std::vector<std::uint64_t> &offsets);
	Pointer getFirstValidDoor();
	void setHooks();
	void removeHooks();
public:
	enum class Difficulty : std::uint32_t { Amateur, Easy = 3, Normal = 5, Professional = 6 };
	enum class TypewriterMode : std::uint32_t { Load, Save };
	struct ItemData;
	struct WeaponData;
	Game();
	~Game();
	bool good();

	void setHealth(std::uint16_t health);
	std::uint16_t getHealth();
	void setHealthLimit(std::uint16_t limit);
	std::uint16_t getHealthLimit();

	decltype(items)::ValueType getItemName(const decltype(items)::KeyType &id) const; //throws out_of_range
	decltype(items)::KeyType getItemId(const decltype(items)::ValueType &id) const; //throws out_of_range
	std::vector<ItemData> getInventory() const;
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

	WeaponData* getWeaponDataPtr(std::uint16_t id) const;
	void setWeaponDataPtr(WeaponData *target, const WeaponData &source, const float(&newFirepower)[7]);
	bool isWeapon(std::uint16_t id) const;
	float* getFirepowerTableEntry(std::uint8_t i) const;
	void setFirepowerTableEntry(std::uint8_t i, const float newValues[7]);
	static const std::vector<std::uint16_t>& getAmmoItemIds();

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

	InventoryIconData getItemDimensions(std::uint16_t id);
	void setMaxItemAmount(std::uint16_t id, std::uint32_t amount);

	void toggleFastTmp(bool toggle);
	bool isFastTmpEnabled();

	void loadSceneFile(const std::string &sceneName);

	//returns old callback
	auto setLoggerCallback(void (__cdecl *callback)(const char*, ...)) -> void(__cdecl*)(const char *, ...);

	void openTypewriter(TypewriterMode mode);
};

struct Game::ItemData //Must be 14 bytes
{
	using uint16 = std::uint16_t;
	using uint8 = std::uint8_t;
	uint16 id;
	uint16 amount; //not ammo
	uint16 valid; //if this is 0, the item will not be in the inventory.
private:
	uint16 FP: 4;
	uint16 FS: 4;
	uint16 RS: 4;
	uint16 CA: 4;
	uint16 ammoCnt; //shift to the right 3 times to get actual ammo
public:
	uint8 posX; //zero based coordinates of the lower right square occupied by the item. higher bits are Y, lower bits are X. Coordinates increase by 2 FSR.
	uint8 posY;
	uint8 rotation; //0: right; 1: bottom; 2: left; 3: top
	uint8 inInventory; //1 when the gun is in inventory, 0 when moved outside of it (to discard it)

	void firingSpeed(uint16 level) { FS = level; }
	uint16 firingSpeed() const { return FS; }
	void firePower(uint16 level) { FP = level; }
	uint16 firePower() const { return FP; }
	void capacity(uint16 level) { CA = level; }
	uint16 capacity() const { return CA; }
	void reloadSpeed(uint16 level) { RS = level; }
	uint16 reloadSpeed() const { return RS; }

	void ammo(uint16 count) { ammoCnt = count << 3; }
	uint16 ammo() const { return ammoCnt >> 3; }
};

struct Game::WeaponData
{
	using uint16 = std::uint16_t;
	using uint8 = std::uint8_t;
	uint16 id; //Weapon ID
	uint8 unknown;
	uint8 firepowerIndex;
	uint8 model;
	uint8 unknown2;
	uint16 ammoItemId; //-1 on rocket launcher
	uint16 capacityValues[7]; //0x8000 (a.k.a 32768) for infinite ammo
};

std::ostream& operator<<(std::ostream &os, const Game::ItemData &data);

#endif