/* === src/menu.c === */
/*
 * Console UI layer for menu display and maintenance.
 * All data operations are delegated to menu_logic.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "menu.h"
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
<<<<<<< HEAD
        for (int i = 0; i < count; i++) {
            const MenuItem *item = menu_getItemByIndex(i);
            printf("  [%02d] %-23s  $%.2f\n",
                   i + 1,
                   item->itemName,
                   (double)item->price);
=======
        for (int i = 0; i < menuCount; i++) {
            printf("  [%02d] %-24s  $%.2f\n",
                   menuItems[i].itemID,
                   menuItems[i].itemName,
                   (double)menuItems[i].price);
>>>>>>> 86c6d703c51bd7954372008c1793e51a2e22a568
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
<<<<<<< HEAD
        for (int i = 0; i < count; i++) {
            const MenuItem *item = menu_getItemByIndex(i);
            printf("  [%02d] ID:%d  %-20s  $%.2f\n",
=======
        for (int i = 0; i < menuCount; i++) {
            printf("  [%02d] ID:%d  %-21s  $%.2f\n",
>>>>>>> 86c6d703c51bd7954372008c1793e51a2e22a568
                   i + 1,
                   item->itemID,
                   item->itemName,
                   (double)item->price);
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

    int result = menu_addItem(name, price);

    if (result == MENU_FULL) {
        printf("  Error: Menu is full!\n");
    } else if (result == MENU_DUP) {
        printf("  Error: Item '%s' already exists.\n", name);
    } else {
        /* result is the new itemID */
        menu_save("data/menu.txt");
        printf("Item '%s' added with ID %d!\n", name, result);
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
