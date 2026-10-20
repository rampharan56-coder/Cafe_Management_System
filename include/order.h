/* === include/order.h === */
#ifndef ORDER_H
#define ORDER_H

#define MAX_ORDERS_ITEMS 10
#define MAX_ORDERS       20

#include "discount.h"

typedef struct {
    int itemID;
    int quantity;
    int sugarOption;  
    int iceOption;   
} OrderItem;

typedef struct {
    int orderID;
    OrderItem items[MAX_ORDERS_ITEMS];
    int itemCount;
    float totalBill;
    char discountCode[CODE_SIZE];  
    float discountRate;              
    float discountSavings;           
} Order;

extern Order orders[MAX_ORDERS];
extern int orderCount;

float calculateBill(Order *order);
void printReceipt(Order *order);
void viewOrderHistory(void);

#endif /* ORDER_H */
