/* === src/gui/gui_admin.c ===
 * Admin dashboard — 2-column button grid, centred layout.
 * No functional changes — layout only.
 */
#include <string.h>
#include "raylib.h"
#include "gui_state.h"
#include "gui_style.h"
#include "gui_common.h"

void DrawAdminDashboardScreen(GuiContext *ctx)
{
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    /* ---------------------------------------------------------------- */
    /*  Title — centred at top                                          */
    /* ---------------------------------------------------------------- */
    const char *title = "ADMIN DASHBOARD";
    int tw = MeasureText(title, FONT_TITLE);
    DrawText(title, (sw - tw) / 2, 40, FONT_TITLE, CLR_TITLE);

    /* ---------------------------------------------------------------- */
    /*  Button grid — 2 columns x 4 rows, centred                       */
    /* ---------------------------------------------------------------- */
    float btnW = 200.0f;
    float btnH = 50.0f;
    float colGap = 24.0f;
    float rowGap = 16.0f;
    int   cols = 2;
    int   rows = 4;

    float gridW = cols * btnW + (cols - 1) * colGap;
    float gridH = rows * btnH + (rows - 1) * rowGap;
    float startX = (sw - gridW) / 2.0f;
    float startY = (sh - gridH) / 2.0f;

    /* Button labels in reading order (row 0: col 0, col 1; row 1: ...) */
    const char *labels[] = {
        "View Menu",     "Add Item",
        "Update Item",   "Delete Item",
        "Order History", "Sales Analytics",
        "Inventory",     "Logout"
    };

    for (int i = 0; i < 8; i++) {
        int col = i % cols;
        int row = i / cols;
        float bx = startX + col * (btnW + colGap);
        float by = startY + row * (btnH + rowGap);

        if (Btn((Rectangle){ bx, by, btnW, btnH }, labels[i])) {
            switch (i) {
                case 0: /* View Menu */
                    GoTo(ctx, SCR_MENU_VIEW);
                    break;
                case 1: /* Add Item */
                    ctx->addNameBuf[0] = '\0';
                    ctx->addPriceBuf[0] = '\0';
                    GoTo(ctx, SCR_MENU_ADD);
                    break;
                case 2: /* Update Item */
                    ctx->updateIdBuf[0] = '\0';
                    ctx->updatePriceBuf[0] = '\0';
                    ctx->updateTarget = NULL;
                    GoTo(ctx, SCR_MENU_UPDATE);
                    break;
                case 3: /* Delete Item */
                    ctx->deleteIdBuf[0] = '\0';
                    ctx->deleteTarget = NULL;
                    ctx->deleteConfirming = false;
                    GoTo(ctx, SCR_MENU_DELETE);
                    break;
                case 4: /* Order History */
                    ctx->historyIndex = 0;
                    GoTo(ctx, SCR_ORDER_HISTORY);
                    break;
                case 5: /* Sales Analytics */
                    ctx->analyticsMonthly = 1;
                    GoTo(ctx, SCR_ANALYTICS);
                    break;
                case 6: /* Inventory */
                    GoTo(ctx, SCR_INVENTORY_VIEW);
                    break;
                case 7: /* Logout */
                    GoTo(ctx, SCR_WELCOME);
                    break;
            }
        }
    }

    /* ---------------------------------------------------------------- */
    /*  Notification — centred below the grid                           */
    /* ---------------------------------------------------------------- */
    if (ctx->message[0]) {
        int mw = MeasureText(ctx->message, FONT_BODY);
        DrawText(ctx->message, (sw - mw) / 2, startY + gridH + 24, FONT_BODY,
                 ctx->messageColor);
    }
}
