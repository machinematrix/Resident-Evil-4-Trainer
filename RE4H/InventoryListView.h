#ifndef __INVENTORYLISTVIEW__
#define __INVENTORYLISTVIEW__
#include "ReportListView.h"
#include "Features.h"

/*
*mantener un contenedor con los items del inventario
*eliminar items borrados en cada llamada a refresh (set_difference y back_inserter)
*agregar items nuevos en cada llamada a refresh (set_difference y back_inserter)
*3 conjuntos: lista, mapa, inventario
*/

class InventoryListView : private ReportListView
{
public:
	using ReportListView::ReportListView;
	using ReportListView::setExtendedStyle;
	using ReportListView::addColumn;
	using ReportListView::move;
	using ReportListView::resize;
	using ReportListView::getColumnCount;
	using ReportListView::setColumnWidth;
	using ReportListView::getSelectedItem;
	using ReportListView::getWindowHandle;
	using ReportListView::itemCount;
	enum InvColumn { Address, Id, Name, Amount, Inventory, FirePower, FiringSpeed, ReloadSpeed, Capacity };
	void refresh();
	void eraseSelectedItem();
	Features::ItemData* getItemAddress(unsigned item);
};

#endif