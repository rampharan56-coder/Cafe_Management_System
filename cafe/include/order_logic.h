/* === include/order_logic.h === */
#ifndef ORDER_LOGIC_H
#define ORDER_LOGIC_H

#define MAX_ORDERS_ITEMS 10
#define MAX_ORDERS       1000

typedef struct {
    int itemID;
    int quantity;
} OrderItem;

typedef struct {
    int       orderID;
    OrderItem items[MAX_ORDERS_ITEMS];
    int       itemCount;
    float     totalBill;
} Order;

/* ================================================================ */
/*  Order lifecycle — pure data operations, no console I/O          */
/* ================================================================ */

/*
 * Initialise a new Order structure.
 * Sets orderID based on current history count + 1.
 * The caller receives a populated Order ready for order_addItem() calls.
 */
void order_init(Order *outOrder);

/* Append an item+quantity to an in-progress order */
void order_addItem(Order *order, int itemID, int quantity);

/* Calculate the total bill by looking up each item's price via menu_logic */
void order_calculateTotal(Order *order);

/*
 * Record a completed order into the global history array.
 * Returns the index in the history array, or -1 if the history is full.
 */
int order_record(const Order *order);

/*
 * Check whether a new order can be added.
 * Returns 1 if there is capacity, 0 if full.
 *
 * This function exists so the UI layer can check capacity without
 * knowing about MAX_ORDERS.  In the future, when the fixed array is
 * replaced with a dynamic container or database, only this function's
 * implementation and order_record() need to change — the UI layer
 * remains untouched.
 */
int order_canAdd(void);

/* ================================================================ */
/*  Query                                                            */
/* ================================================================ */

int  order_getCount(void);
const Order* order_getOrder(int index);  /* NULL if out of range */

/* ================================================================ */
/*  Persistence — file I/O only, no console I/O                      */
/* ================================================================ */

int order_load(const char *filename);  /* Returns number of orders loaded */
int order_save(const char *filename);  /* Returns 1 on success, 0 on failure */

#endif /* ORDER_LOGIC_H */
