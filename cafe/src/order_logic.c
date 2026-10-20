/* === src/order_logic.c === */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "order_logic.h"
#include "menu_logic.h"
#include "fileio.h"

/* ---- Internal state ---- */
static Order orders[MAX_ORDERS];
static int   orderCount = 0;

/* ================================================================ */
/*  Order lifecycle                                                  */
/* ================================================================ */

void order_init(Order *outOrder)
{
    outOrder->orderID   = orderCount + 1;
    outOrder->itemCount = 0;
    outOrder->totalBill = 0.0f;
}

void order_addItem(Order *order, int itemID, int quantity)
{
    if (order->itemCount >= MAX_ORDERS_ITEMS)
        return;  /* Silently refuse — caller should check first */

    order->items[order->itemCount].itemID   = itemID;
    order->items[order->itemCount].quantity = quantity;
    order->itemCount++;
}

void order_calculateTotal(Order *order)
{
    float total = 0.0f;

    for (int i = 0; i < order->itemCount; i++) {
        const MenuItem *item = menu_getItemByID(order->items[i].itemID);
        if (item)
            total += item->price * (float)order->items[i].quantity;
    }

    order->totalBill = total;
}

int order_canAdd(void)
{
    return (orderCount < MAX_ORDERS) ? 1 : 0;
}

int order_record(const Order *order)
{
    if (orderCount >= MAX_ORDERS)
        return -1;

    orders[orderCount] = *order;
    orderCount++;
    return orderCount - 1;
}

/* ================================================================ */
/*  Query                                                            */
/* ================================================================ */

int order_getCount(void)
{
    return orderCount;
}

const Order* order_getOrder(int index)
{
    if (index < 0 || index >= orderCount)
        return NULL;
    return &orders[index];
}

/* ================================================================ */
/*  Persistence                                                      */
/* ================================================================ */

int order_save(const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Warning: could not open '%s' for writing.\n", filename);
        return 0;
    }

    for (int i = 0; i < orderCount; i++) {
        fprintf(fp, "ORDER|%d|%d|%.2f\n",
                orders[i].orderID,
                orders[i].itemCount,
                (double)orders[i].totalBill);

        for (int j = 0; j < orders[i].itemCount; j++) {
            fprintf(fp, "ITEM|%d|%d\n",
                    orders[i].items[j].itemID,
                    orders[i].items[j].quantity);
        }
    }

    fclose(fp);
    return 1;
}

int order_load(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return 0;

    orderCount = 0;

    char line[128];
    int  curOrder = -1;

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';

        if (strncmp(line, "ORDER|", 6) == 0) {
            if (orderCount >= MAX_ORDERS)
                break;

            curOrder = orderCount;
            int   id, count;
            float total;

            if (sscanf(line + 6, "%d|%d|%f", &id, &count, &total) == 3) {
                orders[curOrder].orderID   = id;
                orders[curOrder].itemCount = 0;
                orders[curOrder].totalBill = total;
                orderCount++;
            }

        } else if (strncmp(line, "ITEM|", 5) == 0 && curOrder >= 0) {
            Order *o = &orders[curOrder];
            if (o->itemCount >= MAX_ORDERS_ITEMS)
                continue;

            int itemID, qty;
            if (sscanf(line + 5, "%d|%d", &itemID, &qty) == 2) {
                o->items[o->itemCount].itemID   = itemID;
                o->items[o->itemCount].quantity = qty;
                o->itemCount++;
            }
        }
    }

    fclose(fp);
    return orderCount;
}
