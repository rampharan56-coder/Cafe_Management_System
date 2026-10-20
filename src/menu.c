/* === src/menu.c === */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "menu.h"
#include "order.h"
#include "utils.h"
#include "fileio.h"
#include "analytics.h"

MenuItem menuItems[MAX_MENU_ITEMS];
int menuCount = 0;
int nextMenuID = 1;


void initializeMenu(void)
{
    const char *name[]  = {"Americano", "Espresso", "Cappuccino", "Latte",
                             "Mocha", "Matcha Latte", "Coffee Milk"};
    const float price[] = {1.50f, 1.25f, 1.75f, 1.75f, 1.85f, 2.00f, 1.50f};
    int count = (int)(sizeof(name) / sizeof(name[0]));

    for (int i = 0; i < count && i < MAX_MENU_ITEMS; i++) {
        menuItems[i].itemID = nextMenuID++;
        strncpy(menuItems[i].itemName, name[i], NAME_BUFFER - 1);
        menuItems[i].itemName[NAME_BUFFER - 1] = '\0';
        menuItems[i].price = price[i];
        menuItems[i].totalSold   = 0;
        menuItems[i].monthlySold = 0;
        menuCount++;
    }
}

void viewMenu(void)
{
    CLEAR_SCREEN();
    displayMonthlyBestSellerNotice();
    printf("************* MENU ITEMS *************\n");
    printf("=====================================\n");

    if (menuCount == 0) {
        printf("  (no items on the menu)\n");
    } else {
        for (int i = 0; i < menuCount; i++) {
            printf("  [%02d] %-23s  $%.2f\n",
                   i + 1,
                   menuItems[i].itemName,
                   (double)menuItems[i].price);
        }
    }

    printf("=====================================\n");
}

/* Admin view — shows both display number and internal item ID */
void viewMenuAdmin(void)
{
    CLEAR_SCREEN();
    printf("************* MENU ITEMS (Admin) *************\n");
    printf("==============================================\n");

    if (menuCount == 0) {
        printf("  (no items on the menu)\n");
    } else {
        for (int i = 0; i < menuCount; i++) {
            printf("  [%02d] ID:%d  %-20s  $%.2f\n",
                   i + 1,
                   menuItems[i].itemID,
                   menuItems[i].itemName,
                   (double)menuItems[i].price);
        }
    }

    printf("==============================================\n");
}

void addMenuItem(void)
{
    CLEAR_SCREEN();
    if (menuCount >= MAX_MENU_ITEMS) {
        printf(" Error: Menu is full (%d items)! Cannot add more items.\n",
               MAX_MENU_ITEMS);
        waitKey(NULL);
        return;
    }

    MenuItem item;

    /* Name input with cancel support */
    while (1) {
        readString("> enter item name (0 to cancel): ", item.itemName, NAME_BUFFER);

        if (strcmp(item.itemName, "0") == 0) {
            printf("Canceled.\n");
            waitKey(NULL);
            return;
        }

        if (!isValidName(item.itemName)) {
            printf("  Error: Name must contain letters and spaces only!\n");
            continue;
        }

        /* Duplicate check */
        int dup = 0;
        for (int i = 0; i < menuCount; i++) {
            if (strcmp(menuItems[i].itemName, item.itemName) == 0) {
                printf("  Error: item '%s' already exists.\n", item.itemName);
                dup = 1;
                break;
            }
        }
        if (!dup)
            break;
    }

    item.price = readFloat("> enter price: $", 0.01f, 9999.99f);
    item.itemID = nextMenuID++;
    item.totalSold = 0;
    item.monthlySold = 0;

    menuItems[menuCount] = item;
    menuCount++;

    saveMenu("data/menu.txt");

    printf("Item '%s' added with ID %d!\n", item.itemName, item.itemID);
    waitKey(NULL);
}

void updateItem(void)
{
    CLEAR_SCREEN();
    viewMenuAdmin();

    if (menuCount == 0) {
        waitKey(NULL);
        return;
    }

    int id = readInt("> enter item ID to update (0 to cancel): ", 0, 99999);
    if (id == 0) {
        printf("Canceled.\n");
        waitKey(NULL);
        return;
    }

    int index = findItemByID(id);
    if (index == -1) {
        printf("No item with ID %d.\n", id);
        waitKey(NULL);
        return;
    }

    printf("\nCurrent Information\n");
    printf("-------------------\n");
    printf("ID    : %d\n", menuItems[index].itemID);
    printf("Name  : %s\n", menuItems[index].itemName);
    printf("Price : $%.2f\n", (double)menuItems[index].price);

    menuItems[index].price = readFloat("> enter new price: $", 0.01f, 9999.99f);

    saveMenu("data/menu.txt");

    printf("\nItem updated successfully!\n");
    waitKey(NULL);
}

void deleteItem(void)
{
    CLEAR_SCREEN();
    viewMenuAdmin();

    if (menuCount == 0) {
        waitKey(NULL);
        return;
    }

    int id = readInt("> enter item ID to delete (0 to cancel): ", 0, 99999);
    if (id == 0) {
        printf("Cancelled.\n");
        waitKey(NULL);
        return;
    }

    int index = findItemByID(id);
    if (index == -1) {
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

    for (int i = index; i < menuCount - 1; i++)
        menuItems[i] = menuItems[i + 1];

    menuCount--;

    saveMenu("data/menu.txt");

    printf("\nItem deleted successfully!\n");
    waitKey(NULL);
}

int findItemByID(int id)
{
    for (int i = 0; i < menuCount; i++) {
        if (menuItems[i].itemID == id)
            return i;
    }
    return -1;
}

int isValidName(const char *name)
{
    if (!name || name[0] == '\0')
        return 0;

    for (int i = 0; name[i] != '\0'; i++) {
        if (!isalpha((unsigned char)name[i]) && !isspace((unsigned char)name[i]))
            return 0;
    }
    return 1;
}
