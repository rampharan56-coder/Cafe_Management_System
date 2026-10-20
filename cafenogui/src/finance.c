/* === src/finance.c === */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "finance.h"
#include "menu.h"
#include "recipe.h"
#include "ingredient.h"
#include "utils.h"
#include "fileio.h"

float cashBalance;
float restockingExpense;
float discountsGiven;
MenuFinancial itemFinance[MAX_FINANCIAL_ITEMS];
int itemFinanceCount;

void finance_init(void)
{
    cashBalance = INITIAL_CASH;
    restockingExpense = 0;
    discountsGiven = 0;
    itemFinanceCount = 0;
    loadFinance("data/finance.txt");
}

static int findFinanceEntry(int menuItemID)
{
    for (int i = 0; i < itemFinanceCount; i++) {
        if (itemFinance[i].menuItemID == menuItemID)
            return i;
    }
    return -1;
}

static int findOrCreateEntry(int menuItemID)
{
    int idx = findFinanceEntry(menuItemID);
    if (idx != -1)
        return idx;

    if (itemFinanceCount >= MAX_FINANCIAL_ITEMS)
        return -1;

    idx = itemFinanceCount++;
    itemFinance[idx].menuItemID = menuItemID;
    itemFinance[idx].revenue    = 0;
    itemFinance[idx].cost       = 0;
    itemFinance[idx].unitsSold  = 0;
    return idx;
}

void finance_recordOrder(const Order *order)
{
    float finalTotal = order->totalBill - order->discountSavings;
    cashBalance += finalTotal;
    discountsGiven += order->discountSavings;

    for (int i = 0; i < order->itemCount; i++) {
        int menuID = order->items[i].itemID;
        int qty    = order->items[i].quantity;
        float price = 0;
        float costPerServing = recipe_calculateCostPerServing(menuID);

        int idx = findItemByID(menuID);
        if (idx != -1)
            price = menuItems[idx].price;

        int finIdx = findOrCreateEntry(menuID);
        if (finIdx == -1)
            continue;

        itemFinance[finIdx].revenue    += price * qty;
        itemFinance[finIdx].cost       += costPerServing * qty;
        itemFinance[finIdx].unitsSold  += qty;
    }

    saveFinance("data/finance.txt");
}

void finance_recordRestock(int ingredientID, int quantityAdded, float totalCost)
{
    (void)ingredientID;
    (void)quantityAdded;
    cashBalance -= totalCost;
    restockingExpense += totalCost;
    if (cashBalance < 0)
        cashBalance = 0;

    saveFinance("data/finance.txt");
}

float finance_getROI(int menuItemID)
{
    int idx = findFinanceEntry(menuItemID);
    if (idx == -1)
        return -1.0f;

    float cst = itemFinance[idx].cost;
    if (cst == 0.0f)
        return -1.0f;  /* N/A — no ingredient cost to compare against */

    float rev = itemFinance[idx].revenue;
    float pft = rev - cst;
    return (pft / cst) * 100.0f;
}

void finance_displayReport(void)
{
    CLEAR_SCREEN();

    float revenue = 0, expense = 0;
    for (int i = 0; i < itemFinanceCount; i++) {
        revenue += itemFinance[i].revenue;
        expense += itemFinance[i].cost;
    }
    float netRevenue = revenue - discountsGiven;
    float profit   = netRevenue - expense;
    float margin   = (netRevenue > 0) ? (profit / netRevenue * 100.0f) : 0.0f;

    printf("============= FINANCIAL REPORT =============\n");
    printf("=============================================\n\n");
    printf(" Cash Balance                $%10.2f\n", (double)cashBalance);
    printf(" Revenue                     $%10.2f\n", (double)revenue);
    printf(" Discounts Given             $%10.2f\n", (double)discountsGiven);
    printf(" Net Revenue                 $%10.2f\n", (double)netRevenue);
    printf(" Ingredient Consumption Cost $%10.2f\n", (double)expense);
    printf(" Restocking Expense          $%10.2f\n", (double)restockingExpense);
    printf(" Gross Profit                $%10.2f\n", (double)profit);
    printf(" Profit Margin                 %8.2f%%\n", (double)margin);

    /* Per-item breakdown */
    printf("\n---------------------------------------------\n");
    printf(" Per-Menu Financial Analysis:\n");
    printf("---------------------------------------------\n");

    int worstMarginIdx = -1, bestROIIdx = -1, worstROIIdx = -1;
    int bestRevenueIdx = -1, bestGrossProfitIdx = -1;
    float worstMarginVal = 1000000000.0f, bestROIVal = -1000000000.0f, worstROIVal = 1000000000.0f;
    float bestRevenueVal = -1000000000.0f, bestGrossProfitVal = -1000000000.0f;

    for (int i = 0; i < itemFinanceCount; i++) {
        int id = itemFinance[i].menuItemID;
        int menuIdx = findItemByID(id);
        const char *name = (menuIdx != -1) ? menuItems[menuIdx].itemName : "Unknown";
        float rev = itemFinance[i].revenue;
        float cst = itemFinance[i].cost;
        int   sold = itemFinance[i].unitsSold;
        float pft = rev - cst;
        float mg  = (rev > 0) ? (pft / rev * 100.0f) : 0.0f;
        float roi = (cst > 0) ? (pft / cst * 100.0f) : -1.0f;

        printf("\n %s\n", name);
        printf("   Sold:      %d\n", sold);
        printf("   Revenue:   $%.2f\n", (double)rev);
        printf("   Cost:      $%.2f\n", (double)cst);
        printf("   Profit:    $%.2f\n", (double)pft);
        printf("   Margin:    %.2f%%\n", (double)mg);
        if (roi >= 0)
            printf("   ROI:       %.2f%%\n", (double)roi);
        else
            printf("   ROI:       N/A\n");

        if (mg < worstMarginVal && sold > 0) { worstMarginVal = mg; worstMarginIdx = i; }
        if (roi > bestROIVal && cst > 0) { bestROIVal = roi; bestROIIdx = i; }
        if (roi < worstROIVal && roi >= 0 && sold > 0) { worstROIVal = roi; worstROIIdx = i; }
        if (rev > bestRevenueVal) { bestRevenueVal = rev; bestRevenueIdx = i; }
        if (pft > bestGrossProfitVal) { bestGrossProfitVal = pft; bestGrossProfitIdx = i; }
    }

    /* Summary highlights */
    printf("---------------------------------------------\n");
    printf(" Financial Highlights\n");
    printf("---------------------------------------------\n");
    if (bestRevenueIdx != -1) {
        int id = itemFinance[bestRevenueIdx].menuItemID;
        int menuIdx = findItemByID(id);
        const char *name = (menuIdx != -1) ? menuItems[menuIdx].itemName : "Unknown";
        printf(" Highest Revenue:        %s ($%.2f)\n", name, (double)bestRevenueVal);
    }
    if (bestGrossProfitIdx != -1) {
        int id = itemFinance[bestGrossProfitIdx].menuItemID;
        int menuIdx = findItemByID(id);
        const char *name = (menuIdx != -1) ? menuItems[menuIdx].itemName : "Unknown";
        printf(" Highest Gross Profit:   %s ($%.2f)\n", name, (double)bestGrossProfitVal);
    }
    if (bestROIIdx != -1) {
        int id = itemFinance[bestROIIdx].menuItemID;
        int menuIdx = findItemByID(id);
        const char *name = (menuIdx != -1) ? menuItems[menuIdx].itemName : "Unknown";
        printf(" Highest ROI:            %s (%.2f%%)\n", name, (double)bestROIVal);
    }
    if (worstMarginIdx != -1) {
        int id = itemFinance[worstMarginIdx].menuItemID;
        int menuIdx = findItemByID(id);
        const char *name = (menuIdx != -1) ? menuItems[menuIdx].itemName : "Unknown";
        printf(" Lowest Profit Margin:   %s (%.2f%%)\n", name, (double)worstMarginVal);
    }
    if (worstROIIdx != -1) {
        int id = itemFinance[worstROIIdx].menuItemID;
        int menuIdx = findItemByID(id);
        const char *name = (menuIdx != -1) ? menuItems[menuIdx].itemName : "Unknown";
        printf(" Lowest ROI:             %s (%.2f%%)\n", name, (double)worstROIVal);
    }
    printf("=============================================\n");

    waitKey(NULL);
}
