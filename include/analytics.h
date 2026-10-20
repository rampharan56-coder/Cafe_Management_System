/* === include/analytics.h === */
#ifndef ANALYTICS_H
#define ANALYTICS_H

#include "order.h"

void displaySalesAnalytics(void);
void displayMonthlyBestSellerNotice(void);
int  getMonthlyBestSeller(void);
int  getLifetimeBestSeller(void);
void updateSalesStatistics(const Order *order);
void resetMonthlyStatisticsIfNeeded(void);

#endif /* ANALYTICS_H */
