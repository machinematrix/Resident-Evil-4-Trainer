#include "InventoryListView.h"
#include "Utility.h"
#include <sstream>

/*
*recorrer items, si alguno ya no es valido borrarlo. Luego recorrer inventario del juego y agregar items nuevos si los hay, o modificar los que ya estan
*Tengo solo lista e inventario del juego.
*/
void InventoryListView::refresh()
{
	for (unsigned listItem = 0, itemCnt = itemCount(); listItem < itemCnt;) //erase items that are no longer in the inventory
	{
		try {
			Features::ItemData *ptr = (Features::ItemData*)std::stoi(getItemText(listItem, Address), nullptr, 16); //throws
			if (ptr->valid())
				++listItem;
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

	for (Features::ItemData *first = Features::BeginInventory(), *last = Features::EndInventory(); first != last; ++first)
	{
		if (first->valid()) //there are no more invalid items in the list, so this is ok.
		{
			DWORD item;
			std::wostringstream ostream;
			
			ostream << std::hex << first << std::dec;
			item = findItem(ostream.str());
			
			if (item == -1) {
				insertItemBack(ostream.str());
				item = itemCount() - 1;
			}

			setItemText(item, Id, std::to_wstring(static_cast<std::uint16_t>(first->itemId())));
			try {
				setItemText(item, Name, Features::GetItemName(first->itemId()));
			}
			catch (const std::out_of_range&) {
				String msg(TEXT("Name not found for ID "));
				msg += std::to_wstring(static_cast<std::uint32_t>(first->itemId()));
				ErrorBox(GetParent(getWindowHandle()), msg.c_str());
			}
			setItemText(item, Amount, std::to_wstring(first->amount()));
			setItemText(item, Inventory, std::to_wstring(first->inventory()));
			setItemText(item, FirePower, std::to_wstring(first->firePower()));
			setItemText(item, FiringSpeed, std::to_wstring(first->firingSpeed()));
			setItemText(item, ReloadSpeed, std::to_wstring(first->reloadSpeed()));
			setItemText(item, Capacity, std::to_wstring(first->capacity()));
		}
	}
}

void InventoryListView::eraseSelectedItem()
{
	auto item = getSelectedItem();
	if (item != -1)
	{
		try {
			Features::ItemData *itemPtr = (Features::ItemData*)std::stoi(getItemText(item, Address), nullptr, 16);
			itemPtr->valid(0);
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

Features::ItemData* InventoryListView::getItemAddress(unsigned item)
{
	Features::ItemData *result = nullptr;
	if (item < itemCount())
	{
		try {
			result = (Features::ItemData*)std::stoi(getItemText(item, Address), nullptr, 16);
		}
		catch (const std::invalid_argument&) {
			String msg = TEXT("Invalid pointer found in list item ");
			msg += std::to_wstring(item);
			ErrorBox(GetParent(getWindowHandle()), msg.c_str());
		}
	}
	return result;
}