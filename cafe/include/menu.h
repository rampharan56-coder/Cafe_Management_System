/* === include/menu.h === */
#ifndef MENU_H
#define MENU_H

#include "menu_logic.h"  /* MenuItem, getters, etc. */

/*
 * Console-UI functions for menu display and maintenance.
 * These handle all printf/scanf/readInt interactions.
 * Data operations are delegated to menu_logic.
 */

void viewMenu(void);
void viewMenuAdmin(void);
void addMenuItem(void);
void updateItem(void);
void deleteItem(void);
void displayMonthlyBestSellerNotice(void);
void displaySalesAnalytics(void);

/* Inventory management — console UI */
void viewInventory(void);
void restockItem(void);

#endif /* MENU_H */
