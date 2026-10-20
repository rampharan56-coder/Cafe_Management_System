/* === src/fileio.c === */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileio.h"
#include "menu.h"
#include "order.h"
#include "ingredient.h"
#include "recipe.h"
#include "finance.h"
#include "discount.h"


void saveMenu(const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Warning: could not open '%s' for writing.\n", filename);
        return;
    }

    for (int i = 0; i < menuCount; i++) {
        fprintf(fp, "%d|%s|%.2f|%d|%d\n",
                menuItems[i].itemID,
                menuItems[i].itemName,
                (double)menuItems[i].price,
                menuItems[i].totalSold,
                menuItems[i].monthlySold);
    }

    fclose(fp);
}

void loadMenu(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        /* File doesn't exist yet — caller will initializeMenu() + saveMenu() */
        return;
    }

menuCount = 0;
nextMenuID  = 1;

    char line[128];
    while (fgets(line, sizeof(line), fp) && menuCount < MAX_MENU_ITEMS) {
        /* Strip trailing newline */
        {
            char *p = line;
            while (*p && *p != '\n') p++;
            *p = '\0';
        }

        int   id;
        char  name[NAME_BUFFER];
        float price;
        int   totalSold = 0;
        int   monthlySold = 0;
        int   fieldsRead;

        fieldsRead = sscanf(line, "%d|%49[^|]|%f|%d|%d",
                            &id, name, &price, &totalSold, &monthlySold);

        if (fieldsRead == 5 || fieldsRead == 3) {
            menuItems[menuCount].itemID = id;
            strncpy(menuItems[menuCount].itemName, name, NAME_BUFFER - 1);
            menuItems[menuCount].itemName[NAME_BUFFER - 1] = '\0';
            menuItems[menuCount].price = price;
            menuItems[menuCount].totalSold = totalSold;
            menuItems[menuCount].monthlySold = monthlySold;
            menuCount++;

            if (id >= nextMenuID)
                nextMenuID = id + 1;
        }
    }

    fclose(fp);
}


void saveOrders(const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Warning: could not open '%s' for writing.\n", filename);
        return;
    }

    for (int i = 0; i < orderCount; i++) {
        fprintf(fp, "ORDER|%d|%d|%.2f|%.2f|%.2f\n",
                orders[i].orderID,
                orders[i].itemCount,
                (double)orders[i].totalBill,
                (double)orders[i].discountRate,
                (double)orders[i].discountSavings);

        for (int j = 0; j < orders[i].itemCount; j++) {
            fprintf(fp, "ITEM|%d|%d|%d|%d\n",
                    orders[i].items[j].itemID,
                    orders[i].items[j].quantity,
                    orders[i].items[j].sugarOption,
                    orders[i].items[j].iceOption);
        }
    }

    fclose(fp);
}

void loadOrders(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return; 

    orderCount = 0;

    char line[128];
    int  curOrder = -1;

    while (fgets(line, sizeof(line), fp)) {
        /* Strip trailing newline */
        {
            char *p = line;
            while (*p && *p != '\n') p++;
            *p = '\0';
        }

        if (strncmp(line, "ORDER|", 6) == 0) {
            if (orderCount >= MAX_ORDERS)
                break;

            curOrder = orderCount;
            int   id, count;
            float total, discRate = 0, discSavings = 0;
            int fields = sscanf(line + 6, "%d|%d|%f|%f|%f", &id, &count, &total, &discRate, &discSavings);

            if (fields >= 3) {
                orders[curOrder].orderID = id;
                orders[curOrder].itemCount = 0; 
                orders[curOrder].totalBill = total;
                orders[curOrder].discountCode[0] = '\0';
                orders[curOrder].discountRate = (fields >= 4) ? discRate : 0;
                orders[curOrder].discountSavings = (fields >= 5) ? discSavings : 0;
                orderCount++;
            }

        } else if (strncmp(line, "ITEM|", 5) == 0 && curOrder >= 0) {
            Order *o = &orders[curOrder];
            if (o->itemCount >= MAX_ORDERS_ITEMS)
                continue;

            int itemID, qty, sugar = -1, ice = -1;
            int fields = sscanf(line + 5, "%d|%d|%d|%d", &itemID, &qty, &sugar, &ice);
            if (fields >= 2) {
                o->items[o->itemCount].itemID = itemID;
                o->items[o->itemCount].quantity = qty;
                o->items[o->itemCount].sugarOption = (fields >= 3) ? sugar : -1;
                o->items[o->itemCount].iceOption = (fields >= 4) ? ice : -1;
                o->itemCount++;
            }
        }
    }

    fclose(fp);
}


void saveStatistics(const char *filename, int month, int year)
{
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Warning: could not open '%s' for writing.\n", filename);
        return;
    }
    fprintf(fp, "Month=%d\n", month);
    fprintf(fp, "Year=%d\n", year);
    fclose(fp);
}

void loadStatistics(const char *filename, int *month, int *year)
{
    *month = -1;
    *year  = -1;

    FILE *fp = fopen(filename, "r");
    if (!fp)
        return;  

    char line[128];
    while (fgets(line, sizeof(line), fp)) {
        /* Strip trailing newline */
        {
            char *p = line;
            while (*p && *p != '\n') p++;
            *p = '\0';
        }

        char key[64], value[64];
        if (sscanf(line, "%63[^=]=%63s", key, value) == 2) {
            if (strcmp(key, "Month") == 0)
                *month = atoi(value);
            else if (strcmp(key, "Year") == 0)
                *year = atoi(value);
        }
    }

    fclose(fp);
}


void saveAdminPassword(const char *filename, const char *password)
{
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Warning: could not open '%s' for writing.\n", filename);
        return;
    }
    fprintf(fp, "%s\n", password);
    fclose(fp);
}

void loadAdminPassword(const char *filename, char *stored, int size)
{
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        /* First run — create file with default password */
        strncpy(stored, "Adminhere", size - 1);
        stored[size - 1] = '\0';
        saveAdminPassword(filename, stored);
        return;
    }

    if (fgets(stored, size, fp)) {
        /* Strip trailing newline */
        {
            char *p = stored;
            while (*p && *p != '\n') p++;
            *p = '\0';
        }
    } else {
        strncpy(stored, "Adminhere", size - 1);
        stored[size - 1] = '\0';
    }

    fclose(fp);
}


void saveIngredients(const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Warning: could not open '%s' for writing.\n", filename);
        return;
    }

    for (int i = 0; i < ingredientCount; i++) {
        fprintf(fp, "%d|%s|%s|%.4f|%d|%d\n",
                ingredients[i].ingredientID,
                ingredients[i].name,
                ingredients[i].unit,
                (double)ingredients[i].purchasePrice,
                ingredients[i].currentStock,
                ingredients[i].minStock);
    }

    fclose(fp);
}

void loadIngredients(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return;

    ingredientCount = 0;
    nextIngredientID = 1;

    char line[256];
    while (fgets(line, sizeof(line), fp) && ingredientCount < MAX_INGREDIENTS) {
        /* Strip trailing newline */
        {
            char *p = line;
            while (*p && *p != '\n') p++;
            *p = '\0';
        }

        int   id;
        char  name[INGREDIENT_NAME_BUFFER];
        char  unit[8];
        float price;
        int   stock, min;

        int fields = sscanf(line, "%d|%49[^|]|%7[^|]|%f|%d|%d",
                            &id, name, unit, &price, &stock, &min);

        if (fields == 6) {
            ingredients[ingredientCount].ingredientID  = id;
            strncpy(ingredients[ingredientCount].name, name, INGREDIENT_NAME_BUFFER - 1);
            ingredients[ingredientCount].name[INGREDIENT_NAME_BUFFER - 1] = '\0';
            strncpy(ingredients[ingredientCount].unit, unit, sizeof(ingredients[ingredientCount].unit) - 1);
            ingredients[ingredientCount].unit[sizeof(ingredients[ingredientCount].unit) - 1] = '\0';
            ingredients[ingredientCount].purchasePrice = price;
            ingredients[ingredientCount].currentStock = stock;
            ingredients[ingredientCount].minStock = min;
            ingredientCount++;

            if (id >= nextIngredientID)
                nextIngredientID = id + 1;
        }
    }

    fclose(fp);
}


void saveRecipes(const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Warning: could not open '%s' for writing.\n", filename);
        return;
    }

    for (int i = 0; i < recipeCount; i++) {
        const Recipe *r = &recipes[i];

        fprintf(fp, "RECIPE|%d|%d\n", r->menuItemID, r->itemCount);

        for (int j = 0; j < r->itemCount; j++) {
            fprintf(fp, "INGR|%d|%.2f\n",
                    r->items[j].ingredientID,
                    (double)r->items[j].quantity);
        }
    }

    fclose(fp);
}

void loadRecipes(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return;

    char line[128];
    int  curMenuItemID = -1;
    RecipeItem tempItems[MAX_RECIPE_ITEMS];
    int  tempCount = 0;

    while (fgets(line, sizeof(line), fp)) {
        {
            char *p = line;
            while (*p && *p != '\n') p++;
            *p = '\0';
        }

        if (strncmp(line, "RECIPE|", 7) == 0) {
            /* Save previous recipe before starting new one */
            if (curMenuItemID != -1 && tempCount > 0)
                recipe_set(curMenuItemID, tempItems, tempCount);

            int menuID, itemCount;
            if (sscanf(line + 7, "%d|%d", &menuID, &itemCount) == 2) {
                curMenuItemID = menuID;
                tempCount = 0;
            }

        } else if (strncmp(line, "INGR|", 5) == 0 && curMenuItemID != -1) {
            int    ingID;
            float  qty;

            if (sscanf(line + 5, "%d|%f", &ingID, &qty) == 2 && tempCount < MAX_RECIPE_ITEMS) {
                tempItems[tempCount].ingredientID = ingID;
                tempItems[tempCount].quantity     = qty;
                tempCount++;
            }
        }
    }

    /* Save last recipe */
    if (curMenuItemID != -1 && tempCount > 0)
        recipe_set(curMenuItemID, tempItems, tempCount);

    fclose(fp);
}


void saveFinance(const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Warning: could not open '%s' for writing.\n", filename);
        return;
    }

    fprintf(fp, "Cash=%.2f\n", (double)cashBalance);
    fprintf(fp, "RestockExpense=%.2f\n", (double)restockingExpense);
    fprintf(fp, "DiscountsGiven=%.2f\n", (double)discountsGiven);
    for (int i = 0; i < itemFinanceCount; i++) {
        fprintf(fp, "Item|%d|%.2f|%.2f|%d\n",
                itemFinance[i].menuItemID,
                (double)itemFinance[i].revenue,
                (double)itemFinance[i].cost,
                itemFinance[i].unitsSold);
    }

    fclose(fp);
}

void loadFinance(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return;

    char line[128];
    while (fgets(line, sizeof(line), fp)) {
        /* Strip trailing newline */
        {
            char *p = line;
            while (*p && *p != '\n') p++;
            *p = '\0';
        }

        char key[64], value[64];
        if (sscanf(line, "%63[^=]=%63s", key, value) == 2) {
            if (strcmp(key, "Cash") == 0) {
                cashBalance = (float)atof(value);
            } else if (strcmp(key, "RestockExpense") == 0) {
                restockingExpense = (float)atof(value);
            } else if (strcmp(key, "DiscountsGiven") == 0) {
                discountsGiven = (float)atof(value);
            }
        } else if (strncmp(line, "Item|", 5) == 0) {
            int    id, sold;
            float  rev, cst;
            if (sscanf(line + 5, "%d|%f|%f|%d", &id, &rev, &cst, &sold) == 4) {
                if (itemFinanceCount < MAX_FINANCIAL_ITEMS) {
                    int idx = itemFinanceCount++;
                    itemFinance[idx].menuItemID = id;
                    itemFinance[idx].revenue    = rev;
                    itemFinance[idx].cost       = cst;
                    itemFinance[idx].unitsSold  = sold;
                }
            }
        }
    }

    fclose(fp);
}


void saveDiscounts(const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Warning: could not open '%s' for writing.\n", filename);
        return;
    }

    for (int i = 0; i < discountCodeCount; i++)
        fprintf(fp, "%s|%.2f\n", discountCodes[i].code, (double)discountCodes[i].rate);

    fclose(fp);
}

void loadDiscounts(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return;

    discountCodeCount = 0;
    char line[128];

    while (fgets(line, sizeof(line), fp) && discountCodeCount < MAX_DISCOUNT_CODES) {
        /* Strip trailing newline */
        {
            char *p = line;
            while (*p && *p != '\n') p++;
            *p = '\0';
        }

        char code[CODE_SIZE];
        float rate;
        if (sscanf(line, "%15[^|]|%f", code, &rate) == 2) {
            strncpy(discountCodes[discountCodeCount].code, code, CODE_SIZE - 1);
            discountCodes[discountCodeCount].code[CODE_SIZE - 1] = '\0';
            discountCodes[discountCodeCount].rate = rate;
            discountCodeCount++;
        }
    }

    fclose(fp);
}
