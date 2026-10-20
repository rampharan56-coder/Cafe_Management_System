/* === include/discount.h === */
#ifndef DISCOUNT_H
#define DISCOUNT_H

#define MAX_DISCOUNT_CODES 10
#define CODE_SIZE 16

#define SMALL_ORDER 20.0f
#define SMALL_DISCOUNT 5.0f
#define BIG_ORDER 50.0f
#define BIG_DISCOUNT 10.0f

typedef struct {
    char code[CODE_SIZE];
    float rate;  /* Percentage, e.g. 10.0 = 10% */
} DiscountCode;

extern DiscountCode discountCodes[MAX_DISCOUNT_CODES];
extern int discountCodeCount;

/* Lifecycle */
void discount_init(void);

/* Admin CRUD */
void discount_viewAll(void);
void discount_add(void);
void discount_update(void);
void discount_delete(void);
void discount_manageMenu(void);

/* Order calculations */
float discount_findRate(const char *code);
float discount_getAutoRate(float total);
float discount_getBestRate(float total, const char *code);
float discount_calculateSavings(float total, float rate);

#endif /* DISCOUNT_H */
