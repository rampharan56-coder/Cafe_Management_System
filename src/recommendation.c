/* === src/recommendation.c === */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "recommendation.h"
#include "finance.h"
#include "menu.h"
#include "recipe.h"
#include "ingredient.h"
#include "analytics.h"
#include "utils.h"


static float getRevenue(int menuItemID)
{
    for (int i = 0; i < itemFinanceCount; i++)
        if (itemFinance[i].menuItemID == menuItemID)
            return itemFinance[i].revenue;
    return 0;
}

static float getGrossProfit(int menuItemID)
{
    for (int i = 0; i < itemFinanceCount; i++)
        if (itemFinance[i].menuItemID == menuItemID)
            return itemFinance[i].revenue - itemFinance[i].cost;
    return 0;
}

static int getUnitsSold(int menuItemID)
{
    for (int i = 0; i < itemFinanceCount; i++)
        if (itemFinance[i].menuItemID == menuItemID)
            return itemFinance[i].unitsSold;
    return 0;
}

/* Inventory availability score for one item (0-100) */
static float inventoryScore(int menuItemID)
{
    const Recipe *r = recipe_get(menuItemID);
    if (!r || r->itemCount == 0)
        return 100.0f; 

    float total = 0;
    for (int i = 0; i < r->itemCount; i++) {
        int idx = findIngredientByID(r->items[i].ingredientID);
        if (idx == -1) continue;

        int stock = ingredients[idx].currentStock;
        int min = ingredients[idx].minStock;

        if (stock >= min)
            total += 100.0f;
        else if (min > 0)
            total += (float)stock / min * 100.0f;
        else
            total += 0;
    }

    return total / r->itemCount;
}

/* Find max value of a metric across all items with finance data */
static float maxRevenue(void)
{
    float m = 0;
    for (int i = 0; i < itemFinanceCount; i++)
        if (itemFinance[i].revenue > m) m = itemFinance[i].revenue;
    return m;
}

static float maxGrossProfit(void)
{
    float m = 0;
    for (int i = 0; i < itemFinanceCount; i++) {
        float p = itemFinance[i].revenue - itemFinance[i].cost;
        if (p > m) m = p;
    }
    return m;
}

static float maxROI(void)
{
    float m = 0;
    for (int i = 0; i < itemFinanceCount; i++) {
        float r = finance_getROI(itemFinance[i].menuItemID);
        if (r > m) m = r;
    }
    return m;
}

static float maxInventory(void)
{
    float m = 0;
    for (int i = 0; i < menuCount; i++) {
        float s = inventoryScore(menuItems[i].itemID);
        if (s > m) m = s;
    }
    return m > 0 ? m : 1.0f;  
}

/* Normalize a value to 0-100 using the group maximum */
static float normalize(float value, float maxVal)
{
    if (maxVal <= 0) return 0;
    float n = value / maxVal * 100.0f;
    return n > 100.0f ? 100.0f : n;
}


float recommendation_calculateScore(int menuItemID)
{
    float rev = getRevenue(menuItemID);
    float profit = getGrossProfit(menuItemID);
    float roi = finance_getROI(menuItemID);
    float inv = inventoryScore(menuItemID);

    float maxRev = maxRevenue();
    float maxGp = maxGrossProfit();
    float maxR = maxROI();
    float maxInv = maxInventory();

    float salesScore = normalize(rev, maxRev);
    float profitScore = normalize(profit, maxGp);
    float roiScore = (roi >= 0) ? normalize(roi, maxR) : 0;
    float invScore = normalize(inv, maxInv);

    return salesScore * 0.30f + profitScore * 0.30f + roiScore    * 0.20f + invScore    * 0.20f;
}

void recommendation_generate(void)
{
    CLEAR_SCREEN();

    int totalItems = menuCount;
    if (totalItems == 0) {
        printf("  No menu items to analyse.\n");
        waitKey(NULL);
        return;
    }

    /* Collect scores for every menu item */
    float scores[MAX_MENU_ITEMS];
    for (int i = 0; i < totalItems; i++)
        scores[i] = recommendation_calculateScore(menuItems[i].itemID);

    /* ----- Find best promotion (highest score) ----- */
    int bestIdx = 0;
    for (int i = 1; i < totalItems; i++)
        if (scores[i] > scores[bestIdx]) bestIdx = i;

    /* ----- Find pricing review candidates (sold > 0, margin < 15%) ----- */
    int priceWarnIdx = -1;
    float worstPriceMargin = 1000000000.0f;
    for (int i = 0; i < totalItems; i++) {
        int id = menuItems[i].itemID;
        int sold = getUnitsSold(id);
        float rev = getRevenue(id);
        float cst = 0;
        for (int j = 0; j < itemFinanceCount; j++)
            if (itemFinance[j].menuItemID == id) { cst = itemFinance[j].cost; break; }
        float margin = (rev > 0) ? ((rev - cst) / rev * 100.0f) : 0;

        if (sold > 0 && margin < worstPriceMargin && margin >= 0) {
            worstPriceMargin = margin;
            priceWarnIdx = i;
        }
    }

    /* ----- Best ROI item ----- */
    int bestROIIdx = -1;
    float bestROIVal = -1;
    for (int i = 0; i < totalItems; i++) {
        float r = finance_getROI(menuItems[i].itemID);
        if (r > bestROIVal) { bestROIVal = r; bestROIIdx = i; }
    }

    /* ----- Slow sellers (sold > 0 but low score) ----- */
    int slowIdx = -1;
    float lowScore = 1000000000.0f;
    for (int i = 0; i < totalItems; i++) {
        if (getUnitsSold(menuItems[i].itemID) > 0 && scores[i] < lowScore) {
            lowScore = scores[i];
            slowIdx = i;
        }
    }

    /* ----- Display ----- */
    printf("=========== BUSINESS RECOMMENDATIONS ===========\n");
    printf("================================================\n");

    /* 1. Recommended Promotion */
    if (bestIdx >= 0) {
        int id = menuItems[bestIdx].itemID;
        printf("\n Recommended Promotion\n");
        printf(" ---------------------\n");
        printf(" %s  (Score: %.1f)\n", menuItems[bestIdx].itemName, (double)scores[bestIdx]);
        printf(" Reason:\n");
        printf("  - High sales performance\n");
        printf("  - Strong gross profit\n");
        if (finance_getROI(id) >= 0)
            printf("  - Good return on investment\n");
        printf("  - Ingredients are well stocked\n");
        printf(" Recommendation: Promote this product to maximise profit.\n");
    }

    /* 2. Pricing Review */
    if (priceWarnIdx >= 0) {
        int id = menuItems[priceWarnIdx].itemID;
        printf("\n Pricing Review\n");
        printf(" --------------\n");
        printf(" %s\n", menuItems[priceWarnIdx].itemName);
        printf(" Reason: Sold %d units but profit margin is only %.2f%%.\n",
               getUnitsSold(id), (double)worstPriceMargin);
        printf(" Recommendation: Consider increasing the selling price.\n");
    }

    /* 3. Inventory Alerts */
    printf("\n Inventory Alert\n");
    printf(" ---------------\n");
    int alertCount = 0;
    for (int i = 0; i < ingredientCount; i++) {
        if (ingredients[i].currentStock < ingredients[i].minStock) {
            printf(" %s  (%d / %d %s)\n",
                   ingredients[i].name,
                   ingredients[i].currentStock,
                   ingredients[i].minStock,
                   ingredients[i].unit);
            printf("  Restock soon.\n");
            alertCount++;
        }
    }
    if (alertCount == 0)
        printf(" All ingredients are adequately stocked.\n");

    /* 4. Slow Selling Products */
    if (slowIdx >= 0) {
        printf("\n Slow Selling\n");
        printf(" ------------\n");
        printf(" %s  (Score: %.1f)\n", menuItems[slowIdx].itemName, (double)lowScore);
        printf(" Recommendation: Consider a promotion or recipe review.\n");
    }

    /* 5. Highest ROI */
    if (bestROIIdx >= 0) {
        printf("\n Highest ROI\n");
        printf(" -----------\n");
        printf(" %s  (%.2f%%)\n", menuItems[bestROIIdx].itemName, (double)bestROIVal);
        printf(" This item generates the greatest return on ingredient investment.\n");
    }

    printf("\n================================================\n");
    waitKey(NULL);
}
