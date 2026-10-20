/* === include/order.h === */
#ifndef ORDER_H
#define ORDER_H

#include "order_logic.h"  /* Order, OrderItem */

/*
 * Console-UI functions for ordering and receipts.
 * These handle all printf/scanf/readInt interactions.
 * Data operations are delegated to order_logic and menu_logic.
 */

void createOrder(void);
void printReceipt(const Order *order);
void viewOrderHistory(void);

#endif /* ORDER_H */
