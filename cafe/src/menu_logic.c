/* === src/menu_logic.c === */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "menu_logic.h"
#include "fileio.h"

/* ---- Internal state ---- */
static MenuItem menuItems[MAX_MENU_ITEMS];
static int      menuCount  = 0;
static int      nextMenuID = 1;

/* ================================================================ */
/*  Helpers (file-static, not exposed in header)                    */
/* ================================================================ */

static void sortBySales(int *indices, int count, int useMonthly)
{
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - 1 - i; j++) {
            int a = useMonthly ? menuItems[indices[j]].monthlySold
                               : menuItems[indices[j]].totalSold;
            int b = useMonthly ? menuItems[indices[j + 1]].monthlySold
                               : menuItems[indices[j + 1]].totalSold;
            if (a < b) {
                int tmp        = indices[j];
                indices[j]     = indices[j + 1];
                indices[j + 1] = tmp;
            }
        }
    }
}

/* ================================================================ */
/*  Initialisation                                                   */
/* ================================================================ */

void menu_initDefaults(void)
{
    const char  *name[]  = {"Americano", "Espresso", "Cappuccino", "Latte",
                             "Mocha", "Matcha Latte", "Coffee Milk"};
    const float  price[] = {1.50f, 1.25f, 1.75f, 1.75f, 1.85f, 2.00f, 1.50f};
    int count = (int)(sizeof(name) / sizeof(name[0]));

    for (int i = 0; i < count && i < MAX_MENU_ITEMS; i++) {
        menuItems[i].itemID = nextMenuID++;
        strncpy(menuItems[i].itemName, name[i], NAME_BUFFER - 1);
        menuItems[i].itemName[NAME_BUFFER - 1] = '\0';
        menuItems[i].price = price[i];
        menuItems[i].totalSold   = 0;
        menuItems[i].monthlySold = 0;
        menuItems[i].stock       = 50;  /* Default stock for new items */
        menuCount++;
    }
}

/* ================================================================ */
/*  CRUD                                                             */
/* ================================================================ */

int menu_addItem(const char *name, float price, int initialStock)
{
    if (menuCount >= MAX_MENU_ITEMS)
        return MENU_FULL;

    /* Duplicate check */
    for (int i = 0; i < menuCount; i++) {
        if (strcmp(menuItems[i].itemName, name) == 0)
            return MENU_DUP;
    }

    if (initialStock < 0)
        initialStock = 0;

    menuItems[menuCount].itemID      = nextMenuID++;
    strncpy(menuItems[menuCount].itemName, name, NAME_BUFFER - 1);
    menuItems[menuCount].itemName[NAME_BUFFER - 1] = '\0';
    menuItems[menuCount].price       = price;
    menuItems[menuCount].totalSold   = 0;
    menuItems[menuCount].monthlySold = 0;
    menuItems[menuCount].stock       = initialStock;

    menuCount++;
    return menuItems[menuCount - 1].itemID;
}

int menu_updatePrice(int itemID, float newPrice)
{
    int idx = menu_findIndexByID(itemID);
    if (idx == -1)
        return MENU_NFOUND;

    menuItems[idx].price = newPrice;
    return MENU_OK;
}

int menu_deleteItem(int itemID)
{
    int idx = menu_findIndexByID(itemID);
    if (idx == -1)
        return MENU_NFOUND;

    for (int i = idx; i < menuCount - 1; i++)
        menuItems[i] = menuItems[i + 1];

    menuCount--;
    return MENU_OK;
}

/* ================================================================ */
/*  Query                                                            */
/* ================================================================ */

int menu_getCount(void)
{
    return menuCount;
}

int menu_findIndexByID(int id)
{
    for (int i = 0; i < menuCount; i++) {
        if (menuItems[i].itemID == id)
            return i;
    }
    return -1;
}

const MenuItem* menu_getItemByIndex(int index)
{
    if (index < 0 || index >= menuCount)
        return NULL;
    return &menuItems[index];
}

const MenuItem* menu_getItemByID(int itemID)
{
    int idx = menu_findIndexByID(itemID);
    if (idx == -1)
        return NULL;
    return &menuItems[idx];
}

/* ================================================================ */
/*  Sales statistics                                                 */
/* ================================================================ */

void menu_updateSalesStats(const Order *order)
{
    for (int i = 0; i < order->itemCount; i++) {
        int menuIndex = menu_findIndexByID(order->items[i].itemID);
        if (menuIndex != -1) {
            menuItems[menuIndex].totalSold   += order->items[i].quantity;
            menuItems[menuIndex].monthlySold += order->items[i].quantity;
        }
    }
}

void menu_resetMonthlyStatsIfNeeded(void)
{
    time_t     now     = time(NULL);
    struct tm *tm_info = localtime(&now);
    int        currentMonth = tm_info->tm_mon + 1;
    int        currentYear  = tm_info->tm_year + 1900;

    int storedMonth, storedYear;
    loadStatistics("data/statistics.txt", &storedMonth, &storedYear);

    if (storedMonth == -1 || storedYear == -1) {
        saveStatistics("data/statistics.txt", currentMonth, currentYear);
        return;
    }

    if (storedMonth != currentMonth || storedYear != currentYear) {
        for (int i = 0; i < menuCount; i++)
            menuItems[i].monthlySold = 0;

        saveStatistics("data/statistics.txt", currentMonth, currentYear);
        menu_save("data/menu.txt");
    }
}

int menu_getMonthlyBestSeller(void)
{
    int bestIdx = -1;
    int bestVal = 0;

    for (int i = 0; i < menuCount; i++) {
        if (menuItems[i].monthlySold > bestVal) {
            bestVal = menuItems[i].monthlySold;
            bestIdx = i;
        }
    }
    return bestIdx;
}

int menu_getLifetimeBestSeller(void)
{
    int bestIdx = -1;
    int bestVal = 0;

    for (int i = 0; i < menuCount; i++) {
        if (menuItems[i].totalSold > bestVal) {
            bestVal = menuItems[i].totalSold;
            bestIdx = i;
        }
    }
    return bestIdx;
}

void menu_getSalesSortedIndices(int *outIndices, int useMonthly)
{
    for (int i = 0; i < menuCount; i++)
        outIndices[i] = i;
    sortBySales(outIndices, menuCount, useMonthly);
}

/* ================================================================ */
/*  Inventory management                                             */
/* ================================================================ */

int menu_getStock(int itemID)
{
    int idx = menu_findIndexByID(itemID);
    if (idx == -1)
        return -1;
    return menuItems[idx].stock;
}

int menu_addStock(int itemID, int quantity)
{
    int idx = menu_findIndexByID(itemID);
    if (idx == -1)
        return MENU_NFOUND;

    if (quantity < 0)
        quantity = 0;

    menuItems[idx].stock += quantity;
    return menuItems[idx].stock;
}

int menu_reduceStock(int itemID, int quantity)
{
    int idx = menu_findIndexByID(itemID);
    if (idx == -1)
        return MENU_NFOUND;

    if (menuItems[idx].stock < quantity)
        return 0;

    menuItems[idx].stock -= quantity;
    return 1;
}

int menu_hasEnoughStock(int itemID, int quantity)
{
    int idx = menu_findIndexByID(itemID);
    if (idx == -1)
        return -1;

    return (menuItems[idx].stock >= quantity) ? 1 : 0;
}

void menu_reduceStockForOrder(const Order *order)
{
    for (int i = 0; i < order->itemCount; i++) {
        int idx = menu_findIndexByID(order->items[i].itemID);
        if (idx != -1) {
            menuItems[idx].stock -= order->items[i].quantity;
            if (menuItems[idx].stock < 0)
                menuItems[idx].stock = 0;
        }
    }
}

/* ================================================================ */
/*  Validation                                                       */
/* ================================================================ */

int menu_isValidName(const char *name)
{
    if (!name || name[0] == '\0')
        return 0;

    for (int i = 0; name[i] != '\0'; i++) {
        if (!isalpha((unsigned char)name[i]) && !isspace((unsigned char)name[i]))
            return 0;
    }
    return 1;
}

/* ================================================================ */
/*  Persistence                                                      */
/* ================================================================ */

int menu_save(const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Warning: could not open '%s' for writing.\n", filename);
        return 0;
    }

    for (int i = 0; i < menuCount; i++) {
        fprintf(fp, "%d|%s|%.2f|%d|%d|%d\n",
                menuItems[i].itemID,
                menuItems[i].itemName,
                (double)menuItems[i].price,
                menuItems[i].totalSold,
                menuItems[i].monthlySold,
                menuItems[i].stock);
    }

    fclose(fp);
    return 1;
}

int menu_load(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return 0;

    menuCount  = 0;
    nextMenuID = 1;

    char line[128];
    while (fgets(line, sizeof(line), fp) && menuCount < MAX_MENU_ITEMS) {
        line[strcspn(line, "\n")] = '\0';

        int   id;
        char  name[NAME_BUFFER];
        float price;
        int   totalSold   = 0;
        int   monthlySold = 0;
        int   stock       = 0;  /* Default for old format files */
        int   fieldsRead;

        /*
         * Try parsing 6 fields (new format with stock).
         * Fall back to 5 (old format without stock) or 3 (oldest format).
         */
        fieldsRead = sscanf(line, "%d|%49[^|]|%f|%d|%d|%d",
                            &id, name, &price, &totalSold, &monthlySold, &stock);

        if (fieldsRead == 6 || fieldsRead == 5 || fieldsRead == 3) {
            menuItems[menuCount].itemID = id;
            strncpy(menuItems[menuCount].itemName, name, NAME_BUFFER - 1);
            menuItems[menuCount].itemName[NAME_BUFFER - 1] = '\0';
            menuItems[menuCount].price = price;
            menuItems[menuCount].totalSold   = totalSold;
            menuItems[menuCount].monthlySold = monthlySold;
            menuItems[menuCount].stock       = stock;  /* 0 for old format */
            menuCount++;

            if (id >= nextMenuID)
                nextMenuID = id + 1;
        }
    }

    fclose(fp);
    return menuCount;
}
