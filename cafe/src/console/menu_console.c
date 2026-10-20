/* === src/menu.c === */
/*
 * Console UI layer for menu display and maintenance.
 * All data operations are delegated to menu_logic.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "menu.h"
#include "menu_logic.h"
#include "utils.h"

/* ================================================================ */
/*  Display — customer view                                          */
/* ================================================================ */

void viewMenu(void)
{
    CLEAR_SCREEN();
    displayMonthlyBestSellerNotice();
    printf("************* MENU ITEMS *************\n");
    printf("=====================================\n");

    int count = menu_getCount();
    if (count == 0) {
        printf("  (no items on the menu)\n");
    } else {
        for (int i = 0; i < count; i++) {
            const MenuItem *item = menu_getItemByIndex(i);
            printf("  [%02d] %-23s  $%.2f  (%d left)\n",
                   i + 1,
                   item->itemName,
                   (double)item->price,
                   item->stock);
        }
    }

    printf("=====================================\n");
}

/* ================================================================ */
/*  Display — admin view                                             */
/* ================================================================ */

void viewMenuAdmin(void)
{
    CLEAR_SCREEN();
    printf("************* MENU ITEMS (Admin) *************\n");
    printf("==============================================\n");

    int count = menu_getCount();
    if (count == 0) {
        printf("  (no items on the menu)\n");
    } else {
        for (int i = 0; i < count; i++) {
            const MenuItem *item = menu_getItemByIndex(i);
            printf("  [%02d] ID:%d  %-20s  $%.2f  Stock: %d\n",
                   i + 1,
                   item->itemID,
                   item->itemName,
                   (double)item->price,
                   item->stock);
        }
    }

    printf("==============================================\n");
}

/* ================================================================ */
/*  Add item                                                         */
/* ================================================================ */

void addMenuItem(void)
{
    CLEAR_SCREEN();

    /* Quick early-exit if menu is full */
    if (menu_getCount() >= MAX_MENU_ITEMS) {
        printf(" Error: Menu is full (%d items)! Cannot add more items.\n",
               MAX_MENU_ITEMS);
        waitKey(NULL);
        return;
    }

    char name[NAME_BUFFER];

    /* Name input with cancel support */
    while (1) {
        readString("> enter item name (0 to cancel): ", name, NAME_BUFFER);

        if (strcmp(name, "0") == 0) {
            printf("Canceled.\n");
            waitKey(NULL);
            return;
        }

        if (!menu_isValidName(name)) {
            printf("  Error: Name must contain letters and spaces only!\n");
            continue;
        }

        break;  /* Name is valid */
    }

    float price = readFloat("> enter price: $", 0.01f, 9999.99f);
    int   stock = readInt("> enter initial stock quantity: ", 0, 99999);

    int result = menu_addItem(name, price, stock);

    if (result == MENU_FULL) {
        printf("  Error: Menu is full!\n");
    } else if (result == MENU_DUP) {
        printf("  Error: Item '%s' already exists.\n", name);
    } else {
        /* result is the new itemID */
        menu_save("data/menu.txt");
        printf("Item '%s' added with ID %d (Stock: %d)!\n", name, result, stock);
    }

    waitKey(NULL);
}

/* ================================================================ */
/*  Update item                                                      */
/* ================================================================ */

void updateItem(void)
{
    CLEAR_SCREEN();
    viewMenuAdmin();

    if (menu_getCount() == 0) {
        waitKey(NULL);
        return;
    }

    int id = readInt("> enter item ID to update (0 to cancel): ", 0, 99999);
    if (id == 0) {
        printf("Canceled.\n");
        waitKey(NULL);
        return;
    }

    const MenuItem *item = menu_getItemByID(id);
    if (!item) {
        printf("No item with ID %d.\n", id);
        waitKey(NULL);
        return;
    }

    printf("\nCurrent Information\n");
    printf("-------------------\n");
    printf("ID    : %d\n", item->itemID);
    printf("Name  : %s\n", item->itemName);
    printf("Price : $%.2f\n", (double)item->price);
    printf("Stock : %d\n", item->stock);

    float newPrice = readFloat("> enter new price: $", 0.01f, 9999.99f);

    menu_updatePrice(id, newPrice);
    menu_save("data/menu.txt");

    printf("\nItem updated successfully!\n");
    waitKey(NULL);
}

/* ================================================================ */
/*  Delete item                                                      */
/* ================================================================ */

void deleteItem(void)
{
    CLEAR_SCREEN();
    viewMenuAdmin();

    if (menu_getCount() == 0) {
        waitKey(NULL);
        return;
    }

    int id = readInt("> enter item ID to delete (0 to cancel): ", 0, 99999);
    if (id == 0) {
        printf("Cancelled.\n");
        waitKey(NULL);
        return;
    }

    const MenuItem *item = menu_getItemByID(id);
    if (!item) {
        printf("\nNo item with ID %d.\n", id);
        waitKey(NULL);
        return;
    }

    int yn = readYesNo("Delete this item? (Y/N): ");
    if (yn != 1) {
        if (yn == 0)
            printf("Cancelled.\n");
        waitKey(NULL);
        return;
    }

    menu_deleteItem(id);
    menu_save("data/menu.txt");

    printf("\nItem deleted successfully!\n");
    waitKey(NULL);
}

/* ================================================================ */
/*  Sales display — console only                                     */
/* ================================================================ */

void displayMonthlyBestSellerNotice(void)
{
    int bestIdx = menu_getMonthlyBestSeller();

    printf("\n");
    printf("==================================\n");

    if (bestIdx == -1) {
        printf("  No sales have been recorded this month.\n");
    } else {
        int bestVal = menu_getItemByIndex(bestIdx)->monthlySold;
        int count = menu_getCount();

        printf("  *** Monthly Best Seller");
        if (bestVal > 0) {
            int tieCount = 0;
            for (int i = 0; i < count; i++) {
                if (menu_getItemByIndex(i)->monthlySold == bestVal)
                    tieCount++;
            }
            if (tieCount > 1)
                printf("s (Tie)");
        }
        printf(" ***\n");

        for (int i = 0; i < count; i++) {
            const MenuItem *item = menu_getItemByIndex(i);
            if (item->monthlySold == bestVal) {
                printf("  %s\n", item->itemName);
            }
        }
        printf("  %d cups sold this month\n", bestVal);
    }

    printf("==================================\n\n");
}

void displaySalesAnalytics(void)
{
    int count = menu_getCount();

    if (count == 0) {
        printf("  No menu items to analyze.\n");
        waitKey(NULL);
        return;
    }

    /* ---- Monthly Sales Ranking ---- */
    CLEAR_SCREEN();
    printf("========== SALES ANALYTICS ==========\n");
    printf("=====================================\n");

    int indices[MAX_MENU_ITEMS];
    menu_getSalesSortedIndices(indices, 1);

    printf("\n         MONTHLY SALES\n");
    printf("-------------------------------------\n");
    printf(" Rank  %-22s  Sold\n", "Item");
    printf("-------------------------------------\n");

    for (int i = 0; i < count; i++) {
        int idx = indices[i];
        const MenuItem *item = menu_getItemByIndex(idx);
        if (item->monthlySold > 0 || i == 0) {
            printf("  %-4d  %-22s  %d\n",
                   i + 1,
                   item->itemName,
                   item->monthlySold);
        }
    }
    printf("-------------------------------------\n");

    int monthlyBest = menu_getMonthlyBestSeller();
    if (monthlyBest != -1) {
        int bestVal = menu_getItemByIndex(monthlyBest)->monthlySold;
        printf("\n     BEST SELLER THIS MONTH\n");

        for (int i = 0; i < count; i++) {
            const MenuItem *item = menu_getItemByIndex(i);
            if (item->monthlySold == bestVal) {
                printf("  %s  (%d cups)\n", item->itemName, item->monthlySold);
            }
        }
    }

    /* ---- Lifetime Sales Ranking ---- */
    menu_getSalesSortedIndices(indices, 0);

    printf("\n         LIFETIME SALES\n");
    printf("-------------------------------------\n");
    printf(" Rank  %-22s  Sold\n", "Item");
    printf("-------------------------------------\n");

    for (int i = 0; i < count; i++) {
        int idx = indices[i];
        const MenuItem *item = menu_getItemByIndex(idx);
        if (item->totalSold > 0 || i == 0) {
            printf("  %-4d  %-22s  %d\n",
                   i + 1,
                   item->itemName,
                   item->totalSold);
        }
    }
    printf("-------------------------------------\n");

    int lifetimeBest = menu_getLifetimeBestSeller();
    if (lifetimeBest != -1) {
        int bestVal = menu_getItemByIndex(lifetimeBest)->totalSold;
        printf("\n     BEST SELLER OF ALL TIME\n");

        for (int i = 0; i < count; i++) {
            const MenuItem *item = menu_getItemByIndex(i);
            if (item->totalSold == bestVal) {
                printf("  %s  (%d cups)\n", item->itemName, item->totalSold);
            }
        }
    }

    printf("\n=====================================\n");
    waitKey(NULL);
}

/* ================================================================ */
/*  Inventory Management — console UI                                */
/* ================================================================ */

void viewInventory(void)
{
    CLEAR_SCREEN();
    printf("============== INVENTORY ===============\n");
    printf("========================================\n");
    printf(" %-4s %-22s %-7s %-6s %s\n",
           "No.", "Item Name", "Price", "Stock", "Warning");
    printf("----------------------------------------\n");

    int count = menu_getCount();
    if (count == 0) {
        printf("  (no items in the menu)\n");
    } else {
        for (int i = 0; i < count; i++) {
            const MenuItem *item = menu_getItemByIndex(i);
            const char *warning = (item->stock <= 5) ? "LOW STOCK" : "";
            printf(" [%02d] %-22s $%.2f  %-6d %s\n",
                   i + 1,
                   item->itemName,
                   (double)item->price,
                   item->stock,
                   warning);
        }
    }

    printf("========================================\n");
}

void restockItem(void)
{
    CLEAR_SCREEN();

    int count = menu_getCount();
    if (count == 0) {
        printf("  No items to restock.\n");
        waitKey(NULL);
        return;
    }

    /* Show inventory first so admin can see what needs restocking */
    viewInventory();

    int displayNum = readInt("\n> enter item number to restock (0 to cancel): ",
                             0, count);

    if (displayNum == 0) {
        printf("Canceled.\n");
        waitKey(NULL);
        return;
    }

    int menuIndex = displayNum - 1;
    const MenuItem *item = menu_getItemByIndex(menuIndex);

    printf("\n  Item: %s  |  Current Stock: %d\n",
           item->itemName, item->stock);

    int quantity = readInt("> enter quantity to add: ", 1, 99999);

    int newStock = menu_addStock(item->itemID, quantity);
    menu_save("data/menu.txt");

    printf("\n  Restocked '%s' with %d units.\n", item->itemName, quantity);
    printf("  New stock level: %d\n", newStock);

    waitKey(NULL);
}
