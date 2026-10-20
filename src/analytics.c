/* === src/analytics.c === */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "analytics.h"
#include "menu.h"
#include "utils.h"
#include "fileio.h"

/* Helper: sort indices by sales value (descending) */
static void sortBySales(int *indices, int count, int useMonthly)
{
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - 1 - i; j++) {
            int a = useMonthly ? menuItems[indices[j]].monthlySold : menuItems[indices[j]].totalSold;
            int b = useMonthly ? menuItems[indices[j + 1]].monthlySold : menuItems[indices[j + 1]].totalSold;
            if (a < b) {
                int tmp = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = tmp;
            }
        }
    }
}

void updateSalesStatistics(const Order *order)
{
    for (int i = 0; i < order->itemCount; i++) {
        int menuIndex = findItemByID(order->items[i].itemID);
        if (menuIndex != -1) {
            menuItems[menuIndex].totalSold += order->items[i].quantity;
            menuItems[menuIndex].monthlySold += order->items[i].quantity;
        }
    }
}

void resetMonthlyStatisticsIfNeeded(void)
{
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    int currentMonth = tm_info->tm_mon + 1;
    int currentYear = tm_info->tm_year + 1900;

    int storedMonth, storedYear;
    loadStatistics("data/statistics.txt", &storedMonth, &storedYear);

    if (storedMonth == -1 || storedYear == -1) {
        saveStatistics("data/statistics.txt", currentMonth, currentYear);
        return;
    }

    if (storedMonth != currentMonth || storedYear != currentYear) {
        for (int i = 0; i < menuCount; i++)
            menuItems[i].monthlySold = 0;

        saveStatistics("data/statistics.txt", currentMonth, currentYear);
        saveMenu("data/menu.txt");
    }
}

int getMonthlyBestSeller(void)
{
    int bestIdx = -1;
    int bestVal = 0;

    for (int i = 0; i < menuCount; i++) {
        if (menuItems[i].monthlySold > bestVal) {
            bestVal = menuItems[i].monthlySold;
            bestIdx = i;
        }
    }
    return bestIdx;
}

int getLifetimeBestSeller(void)
{
    int bestIdx = -1;
    int bestVal = 0;

    for (int i = 0; i < menuCount; i++) {
        if (menuItems[i].totalSold > bestVal) {
            bestVal = menuItems[i].totalSold;
            bestIdx = i;
        }
    }
    return bestIdx;
}

void displayMonthlyBestSellerNotice(void)
{
    int bestIdx = getMonthlyBestSeller();

    printf("\n");
    printf("==================================\n");

    if (bestIdx == -1) {
        printf("  No sales have been recorded this month.\n");
    } else {
        printf("  *** Monthly Best Seller ***\n");
        printf("  %s\n", menuItems[bestIdx].itemName);
        printf("  %d cups sold this month\n", menuItems[bestIdx].monthlySold);
    }

    printf("==================================\n\n");
}

void displaySalesAnalytics(void)
{
    if (menuCount == 0) {
        printf("  No menu items to analyze.\n");
        waitKey(NULL);
        return;
    }

    int indices[MAX_MENU_ITEMS];
    for (int i = 0; i < menuCount; i++)
        indices[i] = i;

    CLEAR_SCREEN();
    printf("========== SALES ANALYTICS ==========\n");
    printf("=====================================\n");

    sortBySales(indices, menuCount, 1);

    printf("\n         MONTHLY SALES\n");
    printf("-------------------------------------\n");
    printf(" Rank  %-22s  Sold\n", "Item");
    printf("-------------------------------------\n");

    for (int i = 0; i < menuCount; i++) {
        int idx = indices[i];
        if (menuItems[idx].monthlySold > 0 || i == 0) {
            printf("  %-4d  %-22s  %d\n",
                   i + 1,
                   menuItems[idx].itemName,
                   menuItems[idx].monthlySold);
        }
    }
    printf("-------------------------------------\n");

    int monthlyBest = getMonthlyBestSeller();
    if (monthlyBest != -1) {
        printf("\n     BEST SELLER THIS MONTH\n");
        printf("  %s\n", menuItems[monthlyBest].itemName);
        printf("  %d cups\n", menuItems[monthlyBest].monthlySold);
    }

    sortBySales(indices, menuCount, 0);

    printf("\n         LIFETIME SALES\n");
    printf("-------------------------------------\n");
    printf(" Rank  %-22s  Sold\n", "Item");
    printf("-------------------------------------\n");

    for (int i = 0; i < menuCount; i++) {
        int idx = indices[i];
        if (menuItems[idx].totalSold > 0 || i == 0) {
            printf("  %-4d  %-22s  %d\n",
                   i + 1,
                   menuItems[idx].itemName,
                   menuItems[idx].totalSold);
        }
    }
    printf("-------------------------------------\n");

    int lifetimeBest = getLifetimeBestSeller();
    if (lifetimeBest != -1) {
        printf("\n     BEST SELLER OF ALL TIME\n");
        printf("  %s\n", menuItems[lifetimeBest].itemName);
        printf("  %d cups\n", menuItems[lifetimeBest].totalSold);
    }

    printf("\n=====================================\n");
    waitKey(NULL);
}
