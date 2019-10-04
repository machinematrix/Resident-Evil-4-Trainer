#include "InventoryListView.h"
#include "Utility.h"
#include <sstream>

/*
*recorrer items, si alguno ya no es valido borrarlo. Luego recorrer inventario del juego y agregar items nuevos si los hay, o modificar los que ya estan
*Tengo solo lista e inventario del juego.
*/
void InventoryListView::refresh(const Game &hacks)
{
	for (unsigned listItem = 0, itemCnt = itemCount(); listItem < itemCnt;) //erase items that are no longer in the inventory
	{
		try {
			Game::ItemData *ptr = (Game::ItemData*)std::stoi(getItemText(listItem, Address), nullptr, 16); //throws
			if (ptr->valid) ++listItem;
			else {
				eraseItem(listItem);
				--itemCnt;
			}
		}
		catch (const std::invalid_argument&) {
			String msg = TEXT("Invalid pointer found in list: ");
			msg += std::to_wstring(listItem);

			ErrorBox(GetParent(getWindowHandle()), msg.c_str());
			++listItem;
		}
	}

	for (Game::ItemData *first = hacks.begInventory(), *last = hacks.endInventory(); first != last; ++first)
	{
		if (first->valid) //there are no more invalid items in the list, so this is ok.
		{
			DWORD item;
			std::wostringstream ostream;
			
			ostream << std::hex << first << std::dec;
			item = findItem(ostream.str());
			
			if (item == -1) {
				insertItemBack(ostream.str());
				item = itemCount() - 1;
			}

			setItemText(item, Id, std::to_wstring(first->id));
			try { setItemText(item, Name, hacks.getItemName(first->id)); }
			catch (const std::out_of_range&) {
				String msg(TEXT("Name not found for ID "));
				msg += std::to_wstring(first->id);
				ErrorBox(GetParent(getWindowHandle()), msg.c_str());
			}
			setItemText(item, Amount, std::to_wstring(first->amount));
			setItemText(item, Valid, std::to_wstring(first->valid));
			setItemText(item, FirePower, std::to_wstring(first->firePower()));
			setItemText(item, FiringSpeed, std::to_wstring(first->firingSpeed()));
			setItemText(item, ReloadSpeed, std::to_wstring(first->reloadSpeed()));
			setItemText(item, Capacity, std::to_wstring(first->capacity()));
			setItemText(item, Ammo, std::to_wstring(first->ammo()));
		}
	}
}

void InventoryListView::eraseSelectedItem()
{
	auto item = getSelectedItem();
	if (item != -1)
	{
		try {
			Game::ItemData *itemPtr = (Game::ItemData*)std::stoi(getItemText(item, Address), nullptr, 16);
			itemPtr->valid = 0;
			eraseItem(item);
		}
		catch (const std::invalid_argument&) {
			String msg = TEXT("Invalid pointer found in list item ");
			msg += std::to_wstring(item);
			ErrorBox(GetParent(getWindowHandle()), msg.c_str());
		}
	}
	else ErrorBox(GetParent(getWindowHandle()), TEXT("Select an item first"));
}

Game::ItemData* InventoryListView::getItemAddress(unsigned item)
{
	Game::ItemData *result = nullptr;
	if (item < itemCount())
	{
		try {
			result = (Game::ItemData*)std::stoi(getItemText(item, Address), nullptr, 16);
		}
		catch (const std::invalid_argument&) {
			String msg = TEXT("Invalid pointer found in list item ");
			msg += std::to_wstring(item);
			ErrorBox(GetParent(getWindowHandle()), msg.c_str());
		}
	}
	return result;
}
