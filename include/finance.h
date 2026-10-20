/* === include/finance.h === */
#ifndef FINANCE_H
#define FINANCE_H

#include "order.h"

#define MAX_FINANCIAL_ITEMS 10
#define INITIAL_CASH       1000.00f

typedef struct {
    int menuItemID;
    float revenue;
    float cost;
    int unitsSold;
} MenuFinancial;

extern float cashBalance;
extern float restockingExpense;
extern float discountsGiven;
extern MenuFinancial itemFinance[MAX_FINANCIAL_ITEMS];
extern int itemFinanceCount;

void finance_init(void);
void finance_recordOrder(const Order *order);
void finance_recordRestock(int ingredientID, int quantityAdded, float totalCost);
void finance_displayReport(void);

/* Query function for ROI — returns percentage, or -1.0f if cost is zero (N/A) */
float finance_getROI(int menuItemID);

#endif /* FINANCE_H */
