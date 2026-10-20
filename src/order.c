/* === src/order.c === */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "order.h"
#include "menu.h"
#include "utils.h"
#include "fileio.h"

Order orders[MAX_ORDERS];
int orderCount = 0;

/* ------------------------------------------------------------------ */

float calculateBill(Order *order)
{
    float total = 0.0f;

    for (int i = 0; i < order->itemCount; i++) {
        int menuIndex = findItemByID(order->items[i].itemID);
        if (menuIndex != -1)
            total += menuItems[menuIndex].price * (float)order->items[i].quantity;
    }

    order->totalBill = total;
    return total;
}

static const char* sugarLabel(int opt)
{
    switch (opt) {
        case 0: return "No Sugar";
        case 1: return "Less Sugar";
        case 2: return "Normal Sugar";
        case 3: return "Extra Sugar";
        default: return "";
    }
}

static const char* iceLabel(int opt)
{
    switch (opt) {
        case 0: return "No Ice";
        case 1: return "Less Ice";
        case 2: return "Normal Ice";
        case 3: return "Extra Ice";
        default: return "";
    }
}

void printReceipt(Order *order)
{
    CLEAR_SCREEN();

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char timeBuffer[26];
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
        int menuIndex = findItemByID(order->items[i].itemID);
        int quantity = order->items[i].quantity;

        if (menuIndex != -1) {
            float subtotal = menuItems[menuIndex].price * (float)quantity;
            printf(" %-22s x%-4d $%-9.2f\n",
                   menuItems[menuIndex].itemName,
                   quantity,
                   (double)subtotal);

            /* Show customization if applicable */
            const char *sg = sugarLabel(order->items[i].sugarOption);
            const char *ic = iceLabel(order->items[i].iceOption);
            if (sg[0] || ic[0]) {
                printf("   ");
                if (sg[0]) printf("Sugar: %s", sg);
                if (sg[0] && ic[0]) printf("  ");
                if (ic[0]) printf("Ice: %s", ic);
                printf("\n");
            }
        } else {
            printf(" %-22s x%-4d %-10s\n", "Unknown/Deleted Item", quantity, "[N/A]");
        }
    }

    printf("------------------------------------------\n");
    float finalTotal = order->totalBill - order->discountSavings;

    if (order->discountRate > 0) {
        printf(" Subtotal                   $%-9.2f\n", (double)order->totalBill);
        if (order->discountCode[0])
            printf(" Discount (%s - %.0f%%)     $%-9.2f\n",
                   order->discountCode, (double)order->discountRate,
                   (double)order->discountSavings);
        else
            printf(" Discount (Auto - %.0f%%)     $%-9.2f\n",
                   (double)order->discountRate, (double)order->discountSavings);
        printf("------------------------------------------\n");
        printf(" %-29s $%-9.2f\n", "FINAL TOTAL:", (double)finalTotal);
    } else {
        printf(" %-29s $%-9.2f\n", "TOTAL BILL:", (double)order->totalBill);
    }
    printf("==========================================\n");
    printf("        Thank you for your visit!         \n");
    printf("             Have a great day!            \n");
    printf("==========================================\n");
}

void viewOrderHistory(void)
{
    CLEAR_SCREEN();
    printf("************* ORDER HISTORY *************\n");
    printf("=========================================\n");

    if (orderCount == 0) {
        printf("  No orders recorded yet.\n");
        printf("=========================================\n");
        waitKey(NULL);
        return;
    }

    for (int i = 0; i < orderCount; i++) {
        printReceipt(&orders[i]);
        printf("\n");
    }

    waitKey(NULL);
}
