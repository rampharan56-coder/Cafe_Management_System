/* === src/main.c === */
#include <stdio.h>
#include <stdlib.h>
#include "menu_logic.h"
#include "order_logic.h"
#include "order.h"    /* for createOrder() */
#include "admin.h"    /* for adminLogin(), adminMenu() */
#include "utils.h"    /* for CLEAR_SCREEN, readInt */

/* ------------------------------------------------------------------ */
/*  Ensure the data/ directory exists (best-effort, portable)         */
/* ------------------------------------------------------------------ */
static void ensureDataDir(void)
{
#ifdef _WIN32
    system("if not exist data mkdir data");
#else
    system("mkdir -p data");
#endif
}

/* ------------------------------------------------------------------ */
/*  Main menu                                                          */
/* ------------------------------------------------------------------ */
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
    menu_load("data/menu.txt");
    if (menu_getCount() == 0) {
        menu_initDefaults();
        menu_save("data/menu.txt");
    }

    /* Orders: load existing history */
    order_load("data/orders.txt");

    /* Statistics: auto-reset monthly counters if month changed */
    menu_resetMonthlyStatsIfNeeded();

    mainMenu();
    return 0;
}
