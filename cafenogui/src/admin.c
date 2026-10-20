/* === src/admin.c === */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "admin.h"
#include "menu.h"
#include "order.h"
#include "utils.h"
#include "fileio.h"
#include "ingredient.h"
#include "analytics.h"
#include "recipe.h"
#include "finance.h"
#include "recommendation.h"
#include "discount.h"

/* ------------------------------------------------------------------ */
/*  Admin login                                                        */
/* ------------------------------------------------------------------ */

bool adminLogin(void)
{
    char storedPassword[PW_BUFFER];
    loadAdminPassword("data/admin.txt", storedPassword, PW_BUFFER);

    int attempts = 3;

    while (attempts > 0) {
        CLEAR_SCREEN();
        printf("\n=== ADMIN LOGIN ===\n");

        char entered[PW_BUFFER];
        readString("> enter password: ", entered, PW_BUFFER);

        if (strcmp(entered, storedPassword) == 0) {
            printf("\nLogin Successful\n");
            waitKey(NULL);
            return true;
        }

        attempts--;

        if (attempts > 0)
            printf("\nWrong Password. %d attempt(s) left.\n", attempts);
    }

    printf("\nToo Many Failed Attempts!\n");
    waitKey(NULL);
    return false;
}

/* ------------------------------------------------------------------ */
/*  Admin menu                                                         */
/* ------------------------------------------------------------------ */

void adminMenu(void)
{
    while (1) {
        CLEAR_SCREEN();
        printf("***************** THE ADMIN OFFICE ******************\n");
        printf("=====================================================\n");
        printf("[1]. DISPLAY ITEMS\n");
        printf("[2]. ADD ITEMS\n");
        printf("[3]. UPDATE ITEMS\n");
        printf("[4]. DELETE ITEMS\n");
        printf("[5]. ORDER HISTORY\n");
        printf("[6]. SALES ANALYTICS\n");
        printf("[7]. VIEW INGREDIENTS\n");
        printf("[8]. ADD INGREDIENT\n");
        printf("[9]. UPDATE INGREDIENT\n");
        printf("[10]. DELETE INGREDIENT\n");
        printf("[11]. RECIPE MANAGEMENT\n");
        printf("[12]. FINANCIAL REPORT\n");
        printf("[13]. RESTOCK INGREDIENT\n");
        printf("[14]. BUSINESS RECOMMENDATIONS\n");
        printf("[15]. DISCOUNT MANAGEMENT\n");
        printf("[0]. BACK\n");
        printf("=====================================================\n");

        int adminChoice = readInt("> enter admin choice: ", 0, 15);

        switch (adminChoice) {
        case 0:
            return;
        case 1:
            viewMenuAdmin();
            waitKey(NULL);
            break;
        case 2:
            addMenuItem();
            break;
        case 3:
            updateItem();
            break;
        case 4:
            deleteItem();
            break;
        case 5:
            viewOrderHistory();
            break;
        case 6:
            displaySalesAnalytics();
            break;
        case 7:
            viewIngredients();
            waitKey(NULL);
            break;
        case 8:
            addIngredient();
            break;
        case 9:
            updateIngredient();
            break;
        case 10:
            deleteIngredient();
            break;
        case 11:
            recipe_manageMenu();
            break;
        case 12:
            finance_displayReport();
            break;
        case 13:
            restockIngredient();
            break;
        case 14:
            recommendation_generate();
            break;
        case 15:
            discount_manageMenu();
            break;
        default:
            printf("\nInvalid choice\n");
            waitKey(NULL);
        }
    }
}
