/* === src/order.c === */
/*
 * Console UI layer for ordering and receipts.
 * All data operations are delegated to order_logic and menu_logic.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "order.h"
#include "menu.h"       /* viewMenu() — console UI call */
#include "menu_logic.h" /* menu_hasEnoughStock(), menu_reduceStockForOrder() */
#include "utils.h"

/* ================================================================ */
/*  Create order                                                     */
/* ================================================================ */

void createOrder(void)
{
    if (menu_getCount() == 0) {
        printf("\nThe menu is currently empty. Please check back later!\n");
        waitKey(NULL);
        return;
    }

    if (!order_canAdd()) {
        printf("\nOrder limit reached. Cannot place more orders.\n");
        waitKey(NULL);
        return;
    }

    Order newOrder;
    order_init(&newOrder);

    int choice_val;

    do {
        if (newOrder.itemCount >= MAX_ORDERS_ITEMS) {
            printf("\nMaximum items per order reached (%d).\n", MAX_ORDERS_ITEMS);
            break;
        }

        viewMenu();

        int displayNum = readInt("> enter item number: ", 1, menu_getCount());
        int menuIndex = displayNum - 1;
        const MenuItem *item = menu_getItemByIndex(menuIndex);

        /* Show current stock for the selected item */
        printf("  [Stock: %d available]\n", item->stock);

        if (item->stock <= 0) {
            printf("  Sorry, this item is out of stock.\n");
            choice_val = readYesNo("\nTry a different item? (Y/N): ");
            if (choice_val == -1)
                break;
            continue;
        }

        int quantity = readInt("> enter Quantity: ", 1, item->stock);

        /*
         * Check stock via business logic before adding to cart.
         * This ensures the check works even if stock changed between
         * the display and the actual add.
         */
        int enough = menu_hasEnoughStock(item->itemID, quantity);
        if (enough == 0) {
            printf("  Insufficient stock! Only %d available.\n", item->stock);
            choice_val = readYesNo("\nTry a different quantity? (Y/N): ");
            if (choice_val == -1)
                break;
            continue;
        } else if (enough == -1) {
            printf("  Error: Item not found.\n");
            break;
        }

        order_addItem(&newOrder, item->itemID, quantity);

        choice_val = readYesNo("\nAdd More Items? (Y/N): ");
        if (choice_val == -1)
            break;

    } while (choice_val == 1);

    if (newOrder.itemCount == 0) {
        printf("No items added. Order cancelled.\n");
        waitKey(NULL);
        return;
    }

    /* --- Finalise the order --- */
    order_calculateTotal(&newOrder);
    order_record(&newOrder);

    /* Reduce stock for every item in the order */
    menu_reduceStockForOrder(&newOrder);

    order_save("data/orders.txt");
    menu_updateSalesStats(&newOrder);
    menu_save("data/menu.txt");

    printReceipt(&newOrder);
    waitKey(NULL);
}

/* ================================================================ */
/*  Receipt                                                          */
/* ================================================================ */

void printReceipt(const Order *order)
{
    CLEAR_SCREEN();

    time_t      t       = time(NULL);
    struct tm  *tm_info = localtime(&t);
    char        timeBuffer[26];
    strftime(timeBuffer, sizeof(timeBuffer), "%d-%m-%Y %H:%M:%S", tm_info);

    printf("\n==========================================\n");
    printf("             * [RECEIPT] * \n");
    printf("==========================================\n");
    printf(" Date/Time: %s\n", timeBuffer);
    printf(" Order ID : #%04d\n", order->orderID);
    printf("------------------------------------------\n");
    printf(" %-22s %-5s %-10s\n", "ITEM", "QTY", "PRICE");
    printf("\n");

    for (int i = 0; i < order->itemCount; i++) {
        const MenuItem *item = menu_getItemByID(order->items[i].itemID);
        int quantity = order->items[i].quantity;

        if (item) {
            float subtotal = item->price * (float)quantity;
            printf(" %-22s x%-4d $%-9.2f\n",
                   item->itemName,
                   quantity,
                   (double)subtotal);
        } else {
            printf(" %-22s x%-4d %-10s\n", "Unknown/Deleted Item", quantity, "[N/A]");
        }
    }

    printf("------------------------------------------\n");
    printf(" %-29s $%-9.2f\n", "TOTAL BILL:", (double)order->totalBill);
    printf("==========================================\n");
    printf("        Thank you for your visit!         \n");
    printf("             Have a great day!            \n");
    printf("==========================================\n");
}

/* ================================================================ */
/*  Order history                                                    */
/* ================================================================ */

void viewOrderHistory(void)
{
    CLEAR_SCREEN();
    printf("************* ORDER HISTORY *************\n");
    printf("=========================================\n");

    int count = order_getCount();
    if (count == 0) {
        printf("  No orders recorded yet.\n");
        printf("=========================================\n");
        waitKey(NULL);
        return;
    }

    for (int i = 0; i < count; i++) {
        printReceipt(order_getOrder(i));
        printf("\n");
    }

    waitKey(NULL);
}
