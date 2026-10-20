/* === src/customer.c === */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "customer.h"
#include "menu.h"
#include "order.h"
#include "utils.h"
#include "fileio.h"
#include "analytics.h"
#include "recipe.h"
#include "ingredient.h"
#include "finance.h"
#include "discount.h"

/* Check if an item is food (no customization) vs drink */
static int isFoodItem(int menuItemID)
{
    int idx = findItemByID(menuItemID);
    if (idx == -1) return 0;
    return (strstr(menuItems[idx].itemName, "Bread") != NULL);
}

/* Ask customer for drink customization */
static void askDrinkCustomization(DrinkCustomization *custom)
{
    custom->sugarOption = -1;
    custom->iceOption = -1;

    printf("\n--- Sugar ---\n");
    printf("[1] No Sugar\n");
    printf("[2] Less Sugar\n");
    printf("[3] Normal Sugar\n");
    printf("[4] Extra Sugar\n");
    int sg = readInt("> choose sugar: ", 1, 4);
    custom->sugarOption = sg - 1;

    printf("\n--- Ice ---\n");
    printf("[1] No Ice\n");
    printf("[2] Less Ice\n");
    printf("[3] Normal Ice\n");
    printf("[4] Extra Ice\n");
    int ic = readInt("> choose ice: ", 1, 4);
    custom->iceOption = ic - 1;
}

void createOrder(void)
{
    if (menuCount == 0) {
        printf("\nThe menu is currently empty. Please check back later!\n");
        waitKey(NULL);
        return;
    }

    if (orderCount >= MAX_ORDERS) {
        printf("\nOrder limit reached. Cannot place more orders.\n");
        waitKey(NULL);
        return;
    }

    Order newOrder;
    newOrder.orderID = orderCount + 1;
    newOrder.itemCount = 0;
    newOrder.totalBill = 0.0f;

    /* Temporary storage for customized ingredient lists per item */
    RecipeItem customizedItems[MAX_ORDERS_ITEMS][MAX_RECIPE_ITEMS];
    int        customizedCounts[MAX_ORDERS_ITEMS];

    int choice_val;

    do {
        if (newOrder.itemCount >= MAX_ORDERS_ITEMS) {
            printf("\nMaximum items per order reached (%d).\n", MAX_ORDERS_ITEMS);
            break;
        }

        viewMenu();

        int displayNum = readInt("> enter item number: ", 1, menuCount);
        int menuIndex = displayNum - 1;
        int menuItemID = menuItems[menuIndex].itemID;

        /* Check recipe exists */
        if (!recipe_exists(menuItemID)) {
            printf("\n  Sorry, this item has no recipe defined yet.\n");
            printf("  Please choose another item.\n");
            waitKey(NULL);
            continue;
        }

        int quantity = readInt("> enter Quantity: ", 1, 99);

        /* Ask customization for drinks */
        DrinkCustomization custom;
        custom.sugarOption = -1;
        custom.iceOption = -1;

        if (!isFoodItem(menuItemID)) {
            askDrinkCustomization(&custom);
        }

        /* Store order item */
        int idx = newOrder.itemCount;
        newOrder.items[idx].itemID = menuItemID;
        newOrder.items[idx].quantity = quantity;
        newOrder.items[idx].sugarOption = custom.sugarOption;
        newOrder.items[idx].iceOption = custom.iceOption;

        /* Build customized ingredient list (temporary copy — recipe unchanged) */
        customizedCounts[idx] = recipe_applyCustomization(
            menuItemID, &custom, customizedItems[idx], MAX_RECIPE_ITEMS);

        newOrder.itemCount++;

        choice_val = readYesNo("\nAdd More Items? (Y/N): ");

        if (choice_val == -1)
            break;

    } while (choice_val == 1);

    if (newOrder.itemCount == 0) {
        printf("No items added. Order cancelled.\n");
        waitKey(NULL);
        return;
    }

    /* Check stock availability for all items before committing */
    int totalMissing = 0;
    int missingIDs[MAX_RECIPE_ITEMS * MAX_ORDERS_ITEMS];

    for (int i = 0; i < newOrder.itemCount; i++) {
        int m = recipe_checkStock(
            customizedItems[i], customizedCounts[i],
            newOrder.items[i].quantity,
            missingIDs + totalMissing,
            MAX_RECIPE_ITEMS * MAX_ORDERS_ITEMS - totalMissing);
        totalMissing += m;
    }

    if (totalMissing > 0) {
        CLEAR_SCREEN();
        printf("\n==========================================\n");
        printf("   Unable to complete order.\n");
        printf("   Missing ingredients:\n");
        for (int i = 0; i < totalMissing && i < 20; i++) {
            int ingIdx = findIngredientByID(missingIDs[i]);
            if (ingIdx != -1)
                printf("   - %s\n", ingredients[ingIdx].name);
        }
        printf("\n   Please restock before ordering again.\n");
        printf("==========================================\n");
        waitKey(NULL);
        return;
    }

    /* Stock sufficient — commit the order */
    calculateBill(&newOrder);

    /* Ask for discount code */
    newOrder.discountCode[0] = '\0';
    newOrder.discountRate = 0;
    newOrder.discountSavings = 0;

    {
        char code[CODE_SIZE];
        code[0] = '\0';

        printf("\n> enter discount code (or press Enter to skip): ");
        fflush(stdout);
        if (fgets(code, CODE_SIZE, stdin)) {
            /* Strip trailing newline */
            {
                char *p = code;
                while (*p && *p != '\n') p++;
                *p = '\0';
            }
        }

        if (code[0] != '\0') {
            /* Check if the code is valid */
            float codeRate = discount_findRate(code);
            if (codeRate > 0) {
                float bestRate = discount_getBestRate(newOrder.totalBill, code);
                float savings = discount_calculateSavings(newOrder.totalBill, bestRate);

                if (bestRate > codeRate) {
                    /* Auto discount was better */
                    newOrder.discountRate = bestRate;
                    newOrder.discountSavings = savings;
                    printf(" Auto discount applied: %.0f%% (better than code)\n", (double)bestRate);
                } else {
                    strncpy(newOrder.discountCode, code, CODE_SIZE - 1);
                    newOrder.discountCode[CODE_SIZE - 1] = '\0';
                    newOrder.discountRate = bestRate;
                    newOrder.discountSavings = savings;
                    printf(" Coupon discount applied: %.0f%%\n", (double)bestRate);
                }
            } else {
                printf(" Unknown discount code. No discount applied.\n");
            }
        } else {
            /* No code entered — check auto discount */
            float autoRate = discount_getAutoRate(newOrder.totalBill);
            if (autoRate > 0) {
                newOrder.discountRate = autoRate;
                newOrder.discountSavings = discount_calculateSavings(newOrder.totalBill, autoRate);
                printf(" Auto discount: %.0f%% off for orders over $%.0f\n",
                       (double)autoRate,
                       (double)(autoRate == BIG_DISCOUNT ? BIG_ORDER : SMALL_ORDER));
            }
        }
    }

    orders[orderCount] = newOrder;
    orderCount++;

    saveOrders("data/orders.txt");

    /* Consume ingredients using the customized quantities */
    for (int i = 0; i < newOrder.itemCount; i++) {
        recipe_consumeStock(customizedItems[i], customizedCounts[i],
                            newOrder.items[i].quantity);
    }

    updateSalesStatistics(&newOrder);
    saveMenu("data/menu.txt");

    finance_recordOrder(&newOrder);

    printReceipt(&newOrder);
    waitKey(NULL);
}
