/* === src/gui/gui_inventory.c ===
 * Inventory management screens — view stock and restock items.
 * Calls the same menu_logic stock functions as the console UI.
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "raygui.h"
#include "gui_state.h"
#include "gui_style.h"
#include "gui_common.h"

/* ---------------------------------------------------------------- */
/*  View inventory — table with stock levels and low-stock warnings  */
/* ---------------------------------------------------------------- */
void DrawInventoryViewScreen(GuiContext *ctx)
{
    DrawText("INVENTORY", MARGIN, 30, FONT_TITLE, CLR_TEXT);

    int count = menu_getCount();
    int y = 90;

    /* Header */
    DrawText("No.",     MARGIN,     y, FONT_BODY, CLR_MUTED);
    DrawText("Item",    MARGIN+40,  y, FONT_BODY, CLR_MUTED);
    DrawText("Price",   MARGIN+280, y, FONT_BODY, CLR_MUTED);
    DrawText("Stock",   MARGIN+350, y, FONT_BODY, CLR_MUTED);
    DrawText("Warning", MARGIN+420, y, FONT_BODY, CLR_MUTED);
    y += 22;
    DrawLine(MARGIN, y, 700, y, CLR_DIVIDER);
    y += 8;

    if (count == 0) {
        DrawText("(no items in the menu)", MARGIN, y, FONT_BODY, CLR_MUTED);
    } else {
        for (int i = 0; i < count; i++) {
            const MenuItem *item = menu_getItemByIndex(i);
            char buf[16];
            snprintf(buf, sizeof(buf), "%d", i + 1);
            DrawText(buf, MARGIN, y, FONT_BODY, CLR_TEXT);
            DrawText(item->itemName, MARGIN+40, y, FONT_BODY, CLR_TEXT);
            snprintf(buf, sizeof(buf), "$%.2f", (double)item->price);
            DrawText(buf, MARGIN+280, y, FONT_BODY, CLR_TEXT);
            snprintf(buf, sizeof(buf), "%d", item->stock);
            Color sc = (item->stock <= 5) ? CLR_LOW_STOCK : CLR_TEXT;
            DrawText(buf, MARGIN+350, y, FONT_BODY, sc);

            if (item->stock <= 5)
                DrawText("LOW STOCK", MARGIN+420, y, FONT_BODY, CLR_LOW_STOCK);
            y += 24;
        }
    }

    /* Restock button opens the restock screen */
    if (count > 0) {
        if (Btn((Rectangle){MARGIN, 520, 150, 38}, "RESTOCK ITEM")) {
            ctx->invRestockDisplayNum = 1;
            GoTo(ctx, SCR_INVENTORY_RESTOCK);
        }
    }

    if (Btn((Rectangle){MARGIN, 570, 150, 38}, "BACK"))
        GoTo(ctx, SCR_ADMIN_DASHBOARD);

    DrawNotification(ctx, MARGIN, 620);
}

/* ---------------------------------------------------------------- */
/*  Restock — select item by display number and add quantity         */
/* ---------------------------------------------------------------- */
void DrawInventoryRestockScreen(GuiContext *ctx)
{
    DrawText("RESTOCK ITEM", MARGIN, 30, FONT_TITLE, CLR_TEXT);

    int count = menu_getCount();
    if (count == 0) {
        DrawText("No items to restock.", MARGIN, 100, FONT_BODY, CLR_MUTED);
        if (Btn((Rectangle){MARGIN, 560, 150, 38}, "BACK"))
            GoTo(ctx, SCR_INVENTORY_VIEW);
        return;
    }

    /* Quick stock summary so admin can see what needs restocking */
    int y = 80;
    DrawText("No.  Item                    Price    Stock", MARGIN, y, FONT_SMALL, CLR_MUTED);
    y += 18;
    DrawLine(MARGIN, y, 500, y, CLR_DIVIDER);
    y += 6;

    for (int i = 0; i < count; i++) {
        const MenuItem *item = menu_getItemByIndex(i);
        char buf[64];
        snprintf(buf, sizeof(buf), "[%02d] %-22s $%.2f  %d",
                 i + 1, item->itemName, (double)item->price, item->stock);
        Color sc = (item->stock <= 5) ? CLR_LOW_STOCK : CLR_TEXT;
        DrawText(buf, MARGIN, y, FONT_BODY, sc);
        y += 22;
    }

    /* Select item */
    static char itemBuf[8] = "1";
    static bool itemEdit = false;
    DrawText("Item number:", MARGIN, y + 10, FONT_BODY, CLR_TEXT);
    Rectangle itemBox = {MARGIN, (float)y + 32, 80, 30};
    if (GuiTextBox(itemBox, itemBuf, sizeof(itemBuf), itemEdit))
        itemEdit = !itemEdit;

    int displayNum = atoi(itemBuf);
    if (displayNum < 1) displayNum = 1;
    if (displayNum > count) displayNum = count;

    /* Quantity to add */
    static char qtyBuf[8] = "10";
    static bool qtyEdit = false;
    DrawText("Quantity to add:", MARGIN+160, y + 10, FONT_BODY, CLR_TEXT);
    Rectangle qtyBox = {MARGIN+160, (float)y + 32, 80, 30};
    if (GuiTextBox(qtyBox, qtyBuf, sizeof(qtyBuf), qtyEdit))
        qtyEdit = !qtyEdit;

    /* Preview */
    const MenuItem *target = menu_getItemByIndex(displayNum - 1);
    if (target) {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s  --  Current stock: %d  ->  New stock: %d",
                 target->itemName, target->stock,
                 target->stock + atoi(qtyBuf));
        DrawText(buf, MARGIN, (float)y + 80, FONT_BODY, CLR_TEXT);
    }

    /* Confirm button */
    if (Btn((Rectangle){MARGIN, (float)y + 120, 160, 38}, "CONFIRM RESTOCK")) {
        int qty = atoi(qtyBuf);
        if (qty <= 0) {
            SetMessage(ctx, "Enter a quantity greater than 0.", CLR_ERROR);
        } else if (displayNum < 1 || displayNum > count) {
            SetMessage(ctx, "Invalid item number.", CLR_ERROR);
        } else {
            int itemID = menu_getItemByIndex(displayNum - 1)->itemID;
            int newStock = menu_addStock(itemID, qty);
            menu_save("data/menu.txt");
            char buf[96];
            snprintf(buf, sizeof(buf), "Restocked %d units. New stock: %d.", qty, newStock);
            SetMessage(ctx, buf, CLR_SUCCESS);
            qtyBuf[0] = '1'; qtyBuf[1] = '0'; qtyBuf[2] = '\0';
        }
    }

    if (Btn((Rectangle){MARGIN+180, (float)y + 120, 140, 38}, "BACK"))
        GoTo(ctx, SCR_INVENTORY_VIEW);

    DrawNotification(ctx, MARGIN, (float)y + 170);
}
