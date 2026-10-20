/* === src/gui/gui_main.c ===
 * Main GUI loop and Welcome screen.
 * gui_run() creates the window, initialises state, and runs the
 * draw-dispatch loop until the user exits.
 */
#include <string.h>
#include "raylib.h"
#include "raygui.h"
#include "gui.h"
#include "gui_state.h"
#include "gui_style.h"
#include "gui_common.h"

/* Forward declarations of every screen drawer */
static void DrawWelcomeScreen(GuiContext *ctx);
void DrawAdminLoginScreen(GuiContext *ctx);
void DrawAdminDashboardScreen(GuiContext *ctx);
void DrawMenuViewScreen(GuiContext *ctx);
void DrawMenuAddScreen(GuiContext *ctx);
void DrawMenuUpdateScreen(GuiContext *ctx);
void DrawMenuDeleteScreen(GuiContext *ctx);
void DrawOrderHistoryScreen(GuiContext *ctx);
void DrawAnalyticsScreen(GuiContext *ctx);
void DrawCustomerMenuScreen(GuiContext *ctx);
void DrawReceiptScreen(GuiContext *ctx);
void DrawInventoryViewScreen(GuiContext *ctx);
void DrawInventoryRestockScreen(GuiContext *ctx);

/* ---------------------------------------------------------------- */
/*  Welcome screen                                                   */
/* ---------------------------------------------------------------- */
static void DrawWelcomeScreen(GuiContext *ctx)
{
    int w = GetScreenWidth();
    const char *title = "WELCOME TO OUR CAFE STORE";
    int tw = MeasureText(title, FONT_TITLE);
    DrawText(title, (w - tw) / 2, 90, FONT_TITLE, CLR_TITLE);

    float bx = w / 2.0f - 110;
    if (Btn((Rectangle){bx, 220, 220, 50}, "ADMIN")) {
        ctx->adminPasswordBuf[0] = '\0';
        ctx->adminAttemptsLeft = 3;
        ctx->adminPwEditMode = false;
        GoTo(ctx, SCR_ADMIN_LOGIN);
    }
    if (Btn((Rectangle){bx, 290, 220, 50}, "CUSTOMER")) {
        order_init(&ctx->cart);
        for (int i = 0; i < MAX_MENU_ITEMS; i++) ctx->cartQty[i] = 1;
        GoTo(ctx, SCR_CUSTOMER_MENU);
    }
    if (Btn((Rectangle){bx, 360, 220, 50}, "EXIT")) {
        ctx->exitRequested = true;
    }
}

/* ---------------------------------------------------------------- */
/*  Main loop                                                        */
/* ---------------------------------------------------------------- */
void gui_run(void)
{
    GuiContext ctx;
    InitGuiContext(&ctx);

    InitWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE);
    SetTargetFPS(60);
    GuiSetStyle(DEFAULT, TEXT_SIZE, FONT_BODY);

    while (!WindowShouldClose() && !ctx.exitRequested) {
        BeginDrawing();
        ClearBackground(CLR_BG);

        switch (ctx.currentScreen) {
            case SCR_WELCOME:           DrawWelcomeScreen(&ctx);          break;
            case SCR_ADMIN_LOGIN:       DrawAdminLoginScreen(&ctx);       break;
            case SCR_ADMIN_DASHBOARD:   DrawAdminDashboardScreen(&ctx);   break;
            case SCR_MENU_VIEW:         DrawMenuViewScreen(&ctx);         break;
            case SCR_MENU_ADD:          DrawMenuAddScreen(&ctx);          break;
            case SCR_MENU_UPDATE:       DrawMenuUpdateScreen(&ctx);       break;
            case SCR_MENU_DELETE:       DrawMenuDeleteScreen(&ctx);       break;
            case SCR_ORDER_HISTORY:     DrawOrderHistoryScreen(&ctx);     break;
            case SCR_ANALYTICS:         DrawAnalyticsScreen(&ctx);        break;
            case SCR_CUSTOMER_MENU:     DrawCustomerMenuScreen(&ctx);     break;
            case SCR_RECEIPT:           DrawReceiptScreen(&ctx);          break;
            case SCR_INVENTORY_VIEW:    DrawInventoryViewScreen(&ctx);    break;
            case SCR_INVENTORY_RESTOCK: DrawInventoryRestockScreen(&ctx); break;
        }

        EndDrawing();
    }

    CloseWindow();
}

/* ---------------------------------------------------------------- */
/*  Welcome screen — included here since it's part of the loop       */
/*  (already defined above as static)                                */
/* ---------------------------------------------------------------- */
/* (Nothing extra needed — all screen definitions are in their own
   source files under src/gui/ and compiled separately.) */
