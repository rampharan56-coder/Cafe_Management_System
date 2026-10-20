/* === src/console/admin_console.c === */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "admin.h"
#include "menu.h"
#include "order.h"
#include "utils.h"
#include "fileio.h"

/* ------------------------------------------------------------------ */
/*  Admin login (console UI)                                           */
/* ------------------------------------------------------------------ */

bool adminLogin(void)
{
    int attempts = 3;

    while (attempts > 0) {
        CLEAR_SCREEN();
        printf("\n=== ADMIN LOGIN ===\n");

        char entered[PW_BUFFER];
        readString("> enter password: ", entered, PW_BUFFER);

        if (admin_verifyPassword(entered)) {
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
        printf("[7]. INVENTORY MANAGEMENT\n");
        printf("[0]. BACK\n");
        printf("=====================================================\n");

        int adminChoice = readInt("> enter admin choice: ", 0, 7);

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
        case 7: {
            /* Inline inventory submenu */
            while (1) {
                CLEAR_SCREEN();
                printf("*********** INVENTORY MANAGEMENT ***********\n");
                printf("=============================================\n");
                printf("[1]. VIEW CURRENT STOCK\n");
                printf("[2]. RESTOCK ITEM\n");
                printf("[0]. RETURN TO ADMIN MENU\n");
                printf("=============================================\n");
                int invChoice = readInt("> enter choice: ", 0, 2);
                if (invChoice == 0) break;
                else if (invChoice == 1) { viewInventory(); waitKey(NULL); }
                else if (invChoice == 2) { restockItem(); }
            }
            break;
        }
        default:
            printf("\nInvalid choice\n");
            waitKey(NULL);
        }
    }
}
