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
	class ItemData;
	class WeaponData;
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

class Game::ItemData //Must be 14 bytes
{
	std::uint16_t mId;
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
	void itemId(std::uint16_t id);
	std::uint16_t itemId() const;

	void amount(std::uint16_t amount);
	std::uint16_t amount() const;

	void valid(bool valid);
	bool valid() const;

	void firingSpeed(std::uint16_t level);// { FS = level; }
	std::uint16_t firingSpeed() const;// { return FS; }

	void firePower(std::uint16_t level);// { FP = level; }
	std::uint16_t firePower() const;// { return FP; }

	void capacity(std::uint16_t level);// { CA = level; }
	std::uint16_t capacity() const;// { return CA; }

	void reloadSpeed(std::uint16_t level);// { RS = level; }
	std::uint16_t reloadSpeed() const;// { return RS; }

	void ammo(std::uint16_t count);// { mAmmo = count << 3; }
	std::uint16_t ammo() const;// { return mAmmo >> 3; }

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
	std::uint16_t mId; //Weapon ID
	std::uint8_t mUnknown;
	std::uint8_t mFirepowerIndex;
	std::uint8_t mModel;
	std::uint8_t mUnknown2;
	std::uint16_t mAmmoItemId; //-1 on rocket launcher
	std::uint16_t mCapacityValues[capacitySlotCount]; //0x8000 (a.k.a 32768) for infinite ammo

public:

	std::uint16_t id() const;

	void firepowerIndex(std::uint8_t position);
	std::uint8_t firepowerIndex() const;

	void model(std::uint8_t value);
	std::uint8_t model() const;

	void weaponAmmo(std::uint16_t id);
	std::uint16_t weaponAmmo() const;

	void capacity(size_t position, std::uint16_t value);
	std::uint16_t capacity(size_t position) const;
};

std::ostream& operator<<(std::ostream &os, const Game::ItemData &data);

#endif