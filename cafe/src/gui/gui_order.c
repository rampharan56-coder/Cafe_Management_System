/* === src/gui/gui_order.c ===
 * Order history (admin) and Receipt (customer) screens.
 */
#include <string.h>
#include <stdio.h>
#include "raylib.h"
#include "raygui.h"
#include "gui_state.h"
#include "gui_style.h"
#include "gui_common.h"

/* ---------------------------------------------------------------- */
/*  Order history — paginated, one order at a time                   */
/* ---------------------------------------------------------------- */
void DrawOrderHistoryScreen(GuiContext *ctx)
{
    DrawText("ORDER HISTORY", MARGIN, 30, FONT_TITLE, CLR_TEXT);

    int count = order_getCount();
    if (count == 0) {
        DrawText("No orders recorded yet.", MARGIN, 100, FONT_BODY, CLR_MUTED);
        if (Btn((Rectangle){MARGIN, 560, 120, 36}, "BACK")) GoTo(ctx, SCR_ADMIN_DASHBOARD);
        return;
    }

    if (ctx->historyIndex < 0) ctx->historyIndex = 0;
    if (ctx->historyIndex >= count) ctx->historyIndex = count - 1;

    const Order *o = order_getOrder(ctx->historyIndex);

    char buf[64];
    snprintf(buf, sizeof(buf), "Order #%04d  (%d of %d)",
             o->orderID, ctx->historyIndex + 1, count);
    DrawText(buf, MARGIN, 90, FONT_HEAD, CLR_TEXT);

    int y = 130;
    DrawText("Item", MARGIN, y, FONT_BODY, CLR_MUTED);
    DrawText("Qty",     360, y, FONT_BODY, CLR_MUTED);
    DrawText("Subtotal", 440, y, FONT_BODY, CLR_MUTED);
    y += 22;
    DrawLine(MARGIN, y, 600, y, CLR_DIVIDER);
    y += 8;

    for (int i = 0; i < o->itemCount; i++) {
        const MenuItem *item = menu_getItemByID(o->items[i].itemID);
        int qty = o->items[i].quantity;
        char qbuf[8], sbuf[16];
        snprintf(qbuf, sizeof(qbuf), "x%d", qty);

        if (item) {
            DrawText(item->itemName, MARGIN, y, FONT_BODY, CLR_TEXT);
            snprintf(sbuf, sizeof(sbuf), "$%.2f", (double)(item->price * qty));
        } else {
            DrawText("Unknown/Deleted Item", MARGIN, y, FONT_BODY, CLR_MUTED);
            snprintf(sbuf, sizeof(sbuf), "N/A");
        }
        DrawText(qbuf, 360, y, FONT_BODY, CLR_TEXT);
        DrawText(sbuf, 440, y, FONT_BODY, CLR_TEXT);
        y += 24;
    }

    y += 10;
    snprintf(buf, sizeof(buf), "TOTAL: $%.2f", (double)o->totalBill);
    DrawText(buf, MARGIN, y, FONT_HEAD, CLR_SUCCESS);

    if (Btn((Rectangle){MARGIN, 500, 110, 36}, "< PREV")) ctx->historyIndex--;
    if (Btn((Rectangle){160, 500, 110, 36}, "NEXT >")) ctx->historyIndex++;
    if (Btn((Rectangle){MARGIN, 560, 120, 36}, "BACK")) GoTo(ctx, SCR_ADMIN_DASHBOARD);
}

/* ---------------------------------------------------------------- */
/*  Receipt — shown after a successful checkout                      */
/* ---------------------------------------------------------------- */
void DrawReceiptScreen(GuiContext *ctx)
{
    DrawText("RECEIPT", MARGIN, 30, FONT_TITLE, CLR_TEXT);

    if (!ctx->haveLastOrder) {
        DrawText("No receipt to display.", MARGIN, 100, FONT_BODY, CLR_MUTED);
        if (Btn((Rectangle){MARGIN, 560, 150, 38}, "BACK TO MENU")) GoTo(ctx, SCR_WELCOME);
        return;
    }

    char buf[64];
    snprintf(buf, sizeof(buf), "Order ID: #%04d", ctx->lastOrder.orderID);
    DrawText(buf, MARGIN, 90, FONT_HEAD, CLR_TEXT);

    int y = 130;
    DrawText("Item", MARGIN, y, FONT_BODY, CLR_MUTED);
    DrawText("Qty",     360, y, FONT_BODY, CLR_MUTED);
    DrawText("Subtotal", 440, y, FONT_BODY, CLR_MUTED);
    y += 22;
    DrawLine(MARGIN, y, 600, y, CLR_DIVIDER);
    y += 8;

    for (int i = 0; i < ctx->lastOrder.itemCount; i++) {
        const MenuItem *item = menu_getItemByID(ctx->lastOrder.items[i].itemID);
        int qty = ctx->lastOrder.items[i].quantity;
        char qbuf[8], sbuf[16];
        snprintf(qbuf, sizeof(qbuf), "x%d", qty);

        if (item) {
            DrawText(item->itemName, MARGIN, y, FONT_BODY, CLR_TEXT);
            snprintf(sbuf, sizeof(sbuf), "$%.2f", (double)(item->price * qty));
        } else {
            DrawText("Unknown/Deleted Item", MARGIN, y, FONT_BODY, CLR_MUTED);
            snprintf(sbuf, sizeof(sbuf), "N/A");
        }
        DrawText(qbuf, 360, y, FONT_BODY, CLR_TEXT);
        DrawText(sbuf, 440, y, FONT_BODY, CLR_TEXT);
        y += 24;
    }

    y += 10;
    snprintf(buf, sizeof(buf), "TOTAL BILL: $%.2f", (double)ctx->lastOrder.totalBill);
    DrawText(buf, MARGIN, y, FONT_HEAD, CLR_SUCCESS);

    DrawText("Thank you for your visit! Have a great day!",
             MARGIN, y + 40, FONT_BODY, CLR_TEXT);

    if (Btn((Rectangle){MARGIN, 560, 150, 38}, "NEW ORDER")) {
        order_init(&ctx->cart);
        for (int i = 0; i < MAX_MENU_ITEMS; i++) ctx->cartQty[i] = 1;
        GoTo(ctx, SCR_CUSTOMER_MENU);
    }
    if (Btn((Rectangle){210, 560, 150, 38}, "DONE")) GoTo(ctx, SCR_WELCOME);
}
