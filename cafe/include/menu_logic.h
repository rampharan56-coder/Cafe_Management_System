/* === include/menu_logic.h === */
#ifndef MENU_LOGIC_H
#define MENU_LOGIC_H

#include "order_logic.h"  /* For the Order type used in sales statistics */

#define MAX_MENU_ITEMS 10
#define NAME_BUFFER    50

typedef struct {
    int   itemID;
    char  itemName[NAME_BUFFER];
    float price;
    int   totalSold;    /* Lifetime sales — never resets */
    int   monthlySold;  /* Monthly sales — resets automatically */
    int   stock;        /* Current inventory quantity */
} MenuItem;

/*
 * Return codes for CRUD operations.
 * Functions return the new itemID (> 0) on success,
 * or a negative code on failure.
 */
#define MENU_OK      1
#define MENU_FULL   -1
#define MENU_DUP    -2
#define MENU_NFOUND -3  /* item ID not found */
#define MENU_NOSTOCK -4 /* insufficient stock */

/* ================================================================ */
/*  Initialisation                                                   */
/* ================================================================ */

/* Seed the menu array with the default 7 coffee items */
void menu_initDefaults(void);

/* ================================================================ */
/*  CRUD — pure data operations, no console I/O                     */
/* ================================================================ */

/* Add a new item. Returns itemID (> 0) on success, MENU_FULL or MENU_DUP on failure. */
int menu_addItem(const char *name, float price, int initialStock);

/* Update an existing item's price. Returns MENU_OK on success, MENU_NFOUND on failure. */
int menu_updatePrice(int itemID, float newPrice);

/* Delete an item by its internal ID. Returns MENU_OK on success, MENU_NFOUND on failure. */
int menu_deleteItem(int itemID);

/* ================================================================ */
/*  Query                                                            */
/* ================================================================ */

int  menu_getCount(void);
int  menu_findIndexByID(int itemID);
const MenuItem* menu_getItemByIndex(int index);  /* NULL if out of range */
const MenuItem* menu_getItemByID(int itemID);    /* NULL if not found */

/* ================================================================ */
/*  Sales statistics                                                 */
/* ================================================================ */

/* Accumulate sales from a completed order into item totals */
void menu_updateSalesStats(const Order *order);

/* Check saved month/year, reset monthly counters if the calendar month changed */
void menu_resetMonthlyStatsIfNeeded(void);

/* Return the array index of the monthly best seller, or -1 if none */
int menu_getMonthlyBestSeller(void);

/* Return the array index of the lifetime best seller, or -1 if none */
int menu_getLifetimeBestSeller(void);

/*
 * Fill indices[0..menuCount-1] with item-array indices sorted by sales
 * in descending order.  If useMonthly is non-zero, sort by monthlySold;
 * otherwise by totalSold.
 */
void menu_getSalesSortedIndices(int *outIndices, int useMonthly);

/* ================================================================ */
/*  Inventory management — pure data, no console I/O                 */
/* ================================================================ */

/* Get current stock for an item. Returns stock level, or -1 if item not found. */
int menu_getStock(int itemID);

/* Add quantity to stock. Returns new stock level, or MENU_NFOUND on failure. */
int menu_addStock(int itemID, int quantity);

/*
 * Reduce stock by quantity.
 * Returns 1 on success, 0 if insufficient stock, MENU_NFOUND if item not found.
 */
int menu_reduceStock(int itemID, int quantity);

/* Check if enough stock exists. Returns 1 if yes, 0 if no, -1 if item not found. */
int menu_hasEnoughStock(int itemID, int quantity);

/* Reduce stock for every item in a completed order */
void menu_reduceStockForOrder(const Order *order);

/* ================================================================ */
/*  Validation                                                       */
/* ================================================================ */

/* Returns 1 if name contains only letters and spaces, 0 otherwise */
int menu_isValidName(const char *name);

/* ================================================================ */
/*  Persistence — file I/O only, no console I/O                      */
/* ================================================================ */

/* Load menu from file. Returns number of items loaded, or 0 if file absent. */
int menu_load(const char *filename);

/* Save menu to file. Returns 1 on success, 0 on failure. */
int menu_save(const char *filename);

#endif /* MENU_LOGIC_H */
