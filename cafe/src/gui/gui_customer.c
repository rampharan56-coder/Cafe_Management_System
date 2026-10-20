/* === src/gui/gui_customer.c ===
 * Customer ordering screen — browse menu, add items to cart, checkout.
 * Stock is checked before adding; stock is reduced after checkout.
 */
#include <string.h>
#include <stdio.h>
#include "raylib.h"
#include "raygui.h"
#include "gui_state.h"
#include "gui_style.h"
#include "gui_common.h"

void DrawCustomerMenuScreen(GuiContext *ctx)
{
    DrawText("PLACE YOUR ORDER", MARGIN, 30, FONT_TITLE, CLR_TEXT);

    int count = menu_getCount();
    int y    = 65;

    /* ---------------------------------------------------------------- */
    /*  Best Seller Banner                                              */
    /* ---------------------------------------------------------------- */
    {
        int bestIdx = menu_getMonthlyBestSeller();
        const char *badge = "*** Monthly Best Seller ***";

        if (bestIdx >= 0) {
            const MenuItem *best = menu_getItemByIndex(bestIdx);
            char buf[128];
            snprintf(buf, sizeof(buf), "%s  (%d sold this month)",
                     best->itemName, best->monthlySold);
            DrawText(badge, MARGIN, y, FONT_HEAD, CLR_TITLE);
            y += 22;
            DrawText(buf, MARGIN, y, FONT_BODY, CLR_TEXT);
            y += 22;
        } else {
            DrawText(badge, MARGIN, y, FONT_HEAD, CLR_TITLE);
            y += 22;
            DrawText("No sales recorded yet.", MARGIN, y, FONT_BODY, CLR_MUTED);
            y += 22;
        }
    }

    y += 4;
    DrawLine(MARGIN, y, MARGIN + 660, y, CLR_DIVIDER);
    y += 10;

    /* ---------------------------------------------------------------- */
    /*  Menu table                                                      */
    /* ---------------------------------------------------------------- */
    if (count == 0) {
        DrawText("The menu is currently empty. Please check back later!",
                 MARGIN, y, FONT_BODY, CLR_MUTED);
    } else {
        /* Table header */
        DrawText("Item", MARGIN, y, FONT_BODY, CLR_MUTED);
        DrawText("Price", 300, y, FONT_BODY, CLR_MUTED);
        DrawText("Stock", 400, y, FONT_BODY, CLR_MUTED);
        y += 22;
        DrawLine(MARGIN, y, 700, y, CLR_DIVIDER);
        y += 8;

        for (int i = 0; i < count; i++) {
            const MenuItem *item = menu_getItemByIndex(i);
            char buf[96];
            snprintf(buf, sizeof(buf), "%-22s  $%.2f", item->itemName, (double)item->price);
            DrawText(buf, MARGIN, y + 8, FONT_BODY, CLR_TEXT);

            /* Stock display */
            Color sc = (item->stock <= 5) ? CLR_LOW_STOCK : CLR_TEXT;
            snprintf(buf, sizeof(buf), "%d", item->stock);
            DrawText(buf, 400, y + 8, FONT_BODY, sc);

            if (ctx->cartQty[i] < 1) ctx->cartQty[i] = 1;
            char qbuf[8];
            snprintf(qbuf, sizeof(qbuf), "%d", ctx->cartQty[i]);

            if (Btn((Rectangle){480, y, 30, 30}, "-")) {
                if (ctx->cartQty[i] > 1) ctx->cartQty[i]--;
            }
            DrawText(qbuf, 520, y + 6, FONT_BODY, CLR_TEXT);
            if (Btn((Rectangle){550, y, 30, 30}, "+")) {
                if (ctx->cartQty[i] < item->stock && ctx->cartQty[i] < 99)
                    ctx->cartQty[i]++;
            }

            if (Btn((Rectangle){600, y, 110, 30}, "Add to cart")) {
                if (ctx->cart.itemCount >= MAX_ORDERS_ITEMS) {
                    SetMessage(ctx, "Maximum items per order reached.", CLR_ERROR);
                } else if (!menu_hasEnoughStock(item->itemID, ctx->cartQty[i])) {
                    SetMessage(ctx, "Insufficient stock available.", CLR_ERROR);
                } else {
                    order_addItem(&ctx->cart, item->itemID, ctx->cartQty[i]);
                    char buf2[96];
                    snprintf(buf2, sizeof(buf2), "Added %d x %s to cart.",
                             ctx->cartQty[i], item->itemName);
                    SetMessage(ctx, buf2, CLR_SUCCESS);
                }
            }
            y += 36;
        }
    }

    /* Cart summary */
    y += 10;
    DrawLine(MARGIN, y, 700, y, CLR_DIVIDER);
    y += 10;
    DrawText("Cart:", MARGIN, y, FONT_HEAD, CLR_TEXT);
    y += 26;

    if (ctx->cart.itemCount == 0) {
        DrawText("(empty)", MARGIN, y, FONT_BODY, CLR_MUTED);
        y += 24;
    } else {
        for (int i = 0; i < ctx->cart.itemCount; i++) {
            const MenuItem *item = menu_getItemByID(ctx->cart.items[i].itemID);
            char buf[96];
            if (item)
                snprintf(buf, sizeof(buf), "%s  x%d", item->itemName, ctx->cart.items[i].quantity);
            else
                snprintf(buf, sizeof(buf), "Unknown item  x%d", ctx->cart.items[i].quantity);
            DrawText(buf, MARGIN, y, FONT_BODY, CLR_TEXT);
            y += 22;
        }
    }

    if (Btn((Rectangle){MARGIN, 600, 150, 40}, "CHECKOUT")) {
        if (ctx->cart.itemCount == 0) {
            SetMessage(ctx, "No items added. Add something first.", CLR_ERROR);
        } else {
            order_calculateTotal(&ctx->cart);
            order_record(&ctx->cart);
            menu_reduceStockForOrder(&ctx->cart);
            order_save("data/orders.txt");
            menu_updateSalesStats(&ctx->cart);
            menu_save("data/menu.txt");

            ctx->lastOrder = ctx->cart;
            ctx->haveLastOrder = true;
            GoTo(ctx, SCR_RECEIPT);
        }
    }
    if (Btn((Rectangle){210, 600, 150, 40}, "CANCEL")) {
        order_init(&ctx->cart);
        GoTo(ctx, SCR_WELCOME);
    }

    DrawNotification(ctx, 0, 0);
}
