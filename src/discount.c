/* === src/discount.c === */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "discount.h"
#include "utils.h"
#include "fileio.h"

DiscountCode discountCodes[MAX_DISCOUNT_CODES];
int          discountCodeCount;

/* Find a discount by code, return index or -1 */
static int findDiscountIndex(const char *code)
{
    for (int i = 0; i < discountCodeCount; i++) {
        if (strcmp(discountCodes[i].code, code) == 0)
            return i;
    }
    return -1;
}

/* Validate that a code contains only uppercase letters and digits */
static int isValidCode(const char *code)
{
    if (!code || code[0] == '\0')
        return 0;

    for (int i = 0; code[i] != '\0'; i++) {
        if (!isupper((unsigned char)code[i]) && !isdigit((unsigned char)code[i]))
            return 0;
    }
    return 1;
}

void discount_init(void)
{
    discountCodeCount = 0;
    loadDiscounts("data/discounts.txt");

    /* If no discounts exist, seed defaults */
    if (discountCodeCount == 0) {
        strncpy(discountCodes[0].code, "STUDENT10", CODE_SIZE - 1);
        discountCodes[0].code[CODE_SIZE - 1] = '\0';
        discountCodes[0].rate = 10.0f;

        strncpy(discountCodes[1].code, "WELCOME5", CODE_SIZE - 1);
        discountCodes[1].code[CODE_SIZE - 1] = '\0';
        discountCodes[1].rate = 5.0f;

        strncpy(discountCodes[2].code, "CAFE20", CODE_SIZE - 1);
        discountCodes[2].code[CODE_SIZE - 1] = '\0';
        discountCodes[2].rate = 20.0f;

        discountCodeCount = 3;
        saveDiscounts("data/discounts.txt");
    }
}

void discount_viewAll(void)
{
    CLEAR_SCREEN();
    printf("============ DISCOUNT CODES ============\n");
    printf("=========================================\n");

    if (discountCodeCount == 0) {
        printf("  No discount codes configured.\n");
    } else {
        printf(" %-16s %8s\n", "Code", "Rate");
        printf("-----------------------------------------\n");
        for (int i = 0; i < discountCodeCount; i++) {
            printf(" %-16s %7.0f%%\n", discountCodes[i].code, (double)discountCodes[i].rate);
        }
    }

    printf("\n Automatic discounts:\n");
    printf("  Orders over $%.0f  -> %.0f%% off\n", (double)SMALL_ORDER, (double)SMALL_DISCOUNT);
    printf("  Orders over $%.0f -> %.0f%% off\n", (double)BIG_ORDER, (double)BIG_DISCOUNT);
    printf("=========================================\n");
}

void discount_add(void)
{
    CLEAR_SCREEN();
    printf("========== ADD DISCOUNT ==========\n");

    if (discountCodeCount >= MAX_DISCOUNT_CODES) {
        printf("  Error: Maximum discount codes reached (%d).\n", MAX_DISCOUNT_CODES);
        waitKey(NULL);
        return;
    }

    DiscountCode item;
    item.code[0] = '\0';
    item.rate = 0.0f;

    while (1) {
        readString("> enter discount code (0 to cancel): ", item.code, CODE_SIZE);

        if (strcmp(item.code, "0") == 0) {
            printf("Canceled.\n");
            waitKey(NULL);
            return;
        }

        if (!isValidCode(item.code)) {
            printf("  Error: Code must contain only uppercase letters and digits.\n");
            continue;
        }

        if (findDiscountIndex(item.code) != -1) {
            printf("  Error: Discount code '%s' already exists.\n", item.code);
            continue;
        }
        break;
    }

    item.rate = readFloat("> enter discount rate (1-100): ", 1.0f, 100.0f);

    discountCodes[discountCodeCount] = item;
    discountCodeCount++;

    saveDiscounts("data/discounts.txt");

    printf("\nDiscount '%s' added at %.0f%%.\n", item.code, (double)item.rate);
    waitKey(NULL);
}

void discount_update(void)
{
    CLEAR_SCREEN();
    discount_viewAll();

    if (discountCodeCount == 0) {
        waitKey(NULL);
        return;
    }

    char code[CODE_SIZE];
    readString("> enter discount code to update (0 to cancel): ", code, CODE_SIZE);

    if (strcmp(code, "0") == 0) {
        printf("Canceled.\n");
        waitKey(NULL);
        return;
    }

    int idx = findDiscountIndex(code);
    if (idx == -1) {
        printf("  Error: No discount code '%s'.\n", code);
        waitKey(NULL);
        return;
    }

    printf("\nCurrent: %s -> %.0f%%\n", discountCodes[idx].code, (double)discountCodes[idx].rate);

    discountCodes[idx].rate = readFloat("> enter new rate (1-100): ", 1.0f, 100.0f);

    saveDiscounts("data/discounts.txt");

    printf("Discount '%s' updated to %.0f%%.\n", discountCodes[idx].code, (double)discountCodes[idx].rate);
    waitKey(NULL);
}

void discount_delete(void)
{
    CLEAR_SCREEN();
    discount_viewAll();

    if (discountCodeCount == 0) {
        waitKey(NULL);
        return;
    }

    char code[CODE_SIZE];
    readString("> enter discount code to delete (0 to cancel): ", code, CODE_SIZE);

    if (strcmp(code, "0") == 0) {
        printf("Canceled.\n");
        waitKey(NULL);
        return;
    }

    int idx = findDiscountIndex(code);
    if (idx == -1) {
        printf("  Error: No discount code '%s'.\n", code);
        waitKey(NULL);
        return;
    }

    int yn = readYesNo("Delete this discount? (Y/N): ");
    if (yn != 1) {
        printf("Canceled.\n");
        waitKey(NULL);
        return;
    }

    /* Compact the array */
    for (int i = idx; i < discountCodeCount - 1; i++)
        discountCodes[i] = discountCodes[i + 1];
    discountCodeCount--;

    saveDiscounts("data/discounts.txt");

    printf("Discount deleted.\n");
    waitKey(NULL);
}

void discount_manageMenu(void)
{
    while (1) {
        CLEAR_SCREEN();
        printf("============ DISCOUNT MANAGEMENT ============\n");
        printf("=============================================\n");
        printf("[1]. VIEW DISCOUNTS\n");
        printf("[2]. ADD DISCOUNT\n");
        printf("[3]. UPDATE DISCOUNT\n");
        printf("[4]. DELETE DISCOUNT\n");
        printf("[0]. BACK\n");
        printf("=============================================\n");

        int choice = readInt("> enter choice: ", 0, 4);

        switch (choice) {
        case 0:
            return;
        case 1:
            discount_viewAll();
            waitKey(NULL);
            break;
        case 2:
            discount_add();
            break;
        case 3:
            discount_update();
            break;
        case 4:
            discount_delete();
            break;
        default:
            printf("\nInvalid choice.\n");
            waitKey(NULL);
        }
    }
}

/* ----- Order-time calculations ----- */

float discount_findRate(const char *code)
{
    int idx = findDiscountIndex(code);
    if (idx == -1)
        return 0.0f;
    return discountCodes[idx].rate;
}

float discount_getAutoRate(float total)
{
    if (total >= BIG_ORDER)
        return BIG_DISCOUNT;
    if (total >= SMALL_ORDER)
        return SMALL_DISCOUNT;
    return 0.0f;
}

float discount_getBestRate(float total, const char *code)
{
    float codeRate = discount_findRate(code);
    float autoRate = discount_getAutoRate(total);

    if (codeRate >= autoRate)
        return codeRate;
    return autoRate;
}

float discount_calculateSavings(float total, float rate)
{
    return total * (rate / 100.0f);
}
