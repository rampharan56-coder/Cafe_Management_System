/* === src/main.c === */
#include <stdio.h>
#include <stdlib.h>
#include "menu.h"
#include "order.h"
#include "admin.h"
#include "utils.h"
#include "fileio.h"
#include "ingredient.h"
#include "customer.h"
#include "recipe.h"
#include "analytics.h"
#include "finance.h"
#include "discount.h"


static void ensureDataDir(void)
{
#ifdef _WIN32
    system("if not exist data mkdir data");
#else
    system("mkdir -p data");
#endif
}


static void mainMenu(void)
{
    while (1) {
        CLEAR_SCREEN();
        printf("************* WELCOME TO OUR CAFE STORE *************\n");
        printf("=====================================================\n");
        printf("[1]. ADMIN\n");
        printf("[2]. CUSTOMER\n");
        printf("[0]. EXIT\n");
        printf("=====================================================\n");

        int choice = readInt("> enter your choice: ", 0, 2);

        switch (choice) {
        case 1:
            if (adminLogin())
                adminMenu();
            break;
        case 2:
            createOrder();
            break;
        case 0:
            printf("=====================================================\n");
            printf("Thank you for visiting!\n");
            printf("=====================================================\n");
            exit(0);
        }
    }
}

/* ------------------------------------------------------------------ */
/*  Entry point                                                        */
/* ------------------------------------------------------------------ */
int main(void)
{
    ensureDataDir();

    /* Menu: load from file; if absent, seed defaults and save */
    loadMenu("data/menu.txt");
    if (menuCount == 0) {
        initializeMenu();
        saveMenu("data/menu.txt");
    }

    /* Orders: load existing history */
    loadOrders("data/orders.txt");

    /* Statistics: auto-reset monthly counters if month changed */
    resetMonthlyStatisticsIfNeeded();

    /* Ingredients: load from file; if absent, seed defaults and save */
    loadIngredients("data/ingredients.txt");
    if (ingredientCount == 0) {
        initializeIngredients();
        saveIngredients("data/ingredients.txt");
    }

    /* Recipes: load from file if it exists */
    recipe_reset();
    if (recipeCount == 0) {
        initializeDefaultRecipes();
        saveRecipes("data/recipes.txt");
    }

    /* Finance: load existing data or start fresh */
    finance_init();

    /* Discounts: load from file or seed defaults */
    discount_init();

    mainMenu();
    return 0;
}
