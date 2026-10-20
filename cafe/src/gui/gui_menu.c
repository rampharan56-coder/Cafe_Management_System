/* === src/gui/gui_menu.c ===
 * Menu CRUD screens — view, add, update, delete.
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
/*  View menu (admin, read-only)                                     */
/* ---------------------------------------------------------------- */
void DrawMenuViewScreen(GuiContext *ctx)
{
    DrawText("MENU ITEMS", MARGIN, 30, FONT_TITLE, CLR_TEXT);

    int count = menu_getCount();
    int y = 90;
    DrawText("ID",    MARGIN,      y, FONT_BODY, CLR_MUTED);
    DrawText("Name",  MARGIN+60,   y, FONT_BODY, CLR_MUTED);
    DrawText("Price", MARGIN+320,  y, FONT_BODY, CLR_MUTED);
    DrawText("Stock", MARGIN+400,  y, FONT_BODY, CLR_MUTED);
    DrawText("Sold",  MARGIN+480,  y, FONT_BODY, CLR_MUTED);
    y += 24;
    DrawLine(MARGIN, y, 680, y, CLR_DIVIDER);
    y += 8;

    if (count == 0) {
        DrawText("(no items on the menu)", MARGIN, y, FONT_BODY, CLR_MUTED);
    } else {
        for (int i = 0; i < count; i++) {
            const MenuItem *item = menu_getItemByIndex(i);
            char buf[16];
            snprintf(buf, sizeof(buf), "%d", item->itemID);
            DrawText(buf, MARGIN, y, FONT_BODY, CLR_TEXT);
            DrawText(item->itemName, MARGIN+60, y, FONT_BODY, CLR_TEXT);
            snprintf(buf, sizeof(buf), "$%.2f", (double)item->price);
            DrawText(buf, MARGIN+320, y, FONT_BODY, CLR_TEXT);
            snprintf(buf, sizeof(buf), "%d", item->stock);
            Color sc = (item->stock <= 5) ? CLR_LOW_STOCK : CLR_TEXT;
            DrawText(buf, MARGIN+400, y, FONT_BODY, sc);
            snprintf(buf, sizeof(buf), "%d", item->totalSold);
            DrawText(buf, MARGIN+480, y, FONT_BODY, CLR_TEXT);
            y += 24;
        }
    }

    if (Btn((Rectangle){MARGIN, 560, 120, 36}, "BACK")) GoTo(ctx, SCR_ADMIN_DASHBOARD);
}

/* ---------------------------------------------------------------- */
/*  Add item                                                         */
/* ---------------------------------------------------------------- */
void DrawMenuAddScreen(GuiContext *ctx)
{
    DrawText("ADD MENU ITEM", MARGIN, 30, FONT_TITLE, CLR_TEXT);

    DrawText("Name:", MARGIN, 100, FONT_BODY, CLR_TEXT);
    Rectangle nameBox = {MARGIN, 125, 320, 32};
    if (GuiTextBox(nameBox, ctx->addNameBuf, NAME_BUFFER, ctx->addNameEdit))
        ctx->addNameEdit = !ctx->addNameEdit;

    DrawText("Price ($):", MARGIN, 170, FONT_BODY, CLR_TEXT);
    Rectangle priceBox = {MARGIN, 195, 160, 32};
    if (GuiTextBox(priceBox, ctx->addPriceBuf, sizeof(ctx->addPriceBuf), ctx->addPriceEdit))
        ctx->addPriceEdit = !ctx->addPriceEdit;

    DrawText("Initial Stock:", MARGIN, 240, FONT_BODY, CLR_TEXT);
    static char stockBuf[8] = "50";  /* default */
    static bool stockEdit = false;
    Rectangle stockBox = {MARGIN, 265, 100, 32};
    if (GuiTextBox(stockBox, stockBuf, sizeof(stockBuf), stockEdit))
        stockEdit = !stockEdit;

    if (Btn((Rectangle){MARGIN, 315, 140, 38}, "ADD ITEM")) {
        if (!menu_isValidName(ctx->addNameBuf)) {
            SetMessage(ctx, "Name must contain letters and spaces only.", CLR_ERROR);
        } else {
            char *endptr;
            double price = strtod(ctx->addPriceBuf, &endptr);
            int stock = atoi(stockBuf);
            if (stock < 0) stock = 0;
            if (endptr == ctx->addPriceBuf || price <= 0.0 || price > 9999.99) {
                SetMessage(ctx, "Enter a valid price between 0.01 and 9999.99.", CLR_ERROR);
            } else {
                int result = menu_addItem(ctx->addNameBuf, (float)price, stock);
                if (result == MENU_FULL) {
                    SetMessage(ctx, "Menu is full.", CLR_ERROR);
                } else if (result == MENU_DUP) {
                    SetMessage(ctx, "An item with that name already exists.", CLR_ERROR);
                } else {
                    menu_save("data/menu.txt");
                    char buf[128];
                    snprintf(buf, sizeof(buf), "Item '%s' added with ID %d (stock: %d).",
                             ctx->addNameBuf, result, stock);
                    SetMessage(ctx, buf, CLR_SUCCESS);
                    ctx->addNameBuf[0] = '\0';
                    ctx->addPriceBuf[0] = '\0';
                    stockBuf[0] = '5'; stockBuf[1] = '0'; stockBuf[2] = '\0';
                }
            }
        }
    }
    if (Btn((Rectangle){200, 315, 140, 38}, "BACK")) GoTo(ctx, SCR_ADMIN_DASHBOARD);

    DrawNotification(ctx, MARGIN, 370);
}

/* ---------------------------------------------------------------- */
/*  Update item                                                      */
/* ---------------------------------------------------------------- */
void DrawMenuUpdateScreen(GuiContext *ctx)
{
    int sw = GetScreenWidth();
    int count = menu_getCount();
    bool hasItems = (count > 0);

    /* ---------------------------------------------------------------- */
    /*  Title — centred                                                 */
    /* ---------------------------------------------------------------- */
    const char *title = "UPDATE MENU ITEM";
    int tw = MeasureText(title, FONT_TITLE);
    DrawText(title, (sw - tw) / 2, 25, FONT_TITLE, CLR_TITLE);

    /* ---------------------------------------------------------------- */
    /*  Available Menu Items list                                       */
    /* ---------------------------------------------------------------- */
    int y = 65;
    DrawText("Available Menu Items", MARGIN, y, FONT_HEAD, CLR_TEXT);
    y += 24;
    DrawLine(MARGIN, y, MARGIN + 580, y, CLR_DIVIDER);
    y += 8;

    if (!hasItems) {
        DrawText("No menu items available.", MARGIN, y, FONT_BODY, CLR_MUTED);
        y += 24;
    } else {
        for (int i = 0; i < count; i++) {
            const MenuItem *item = menu_getItemByIndex(i);
            char buf[128];
            snprintf(buf, sizeof(buf), "[%d]  %-28s  $%.2f",
                     i + 1, item->itemName, (double)item->price);
            DrawText(buf, MARGIN, y, FONT_BODY, CLR_TEXT);
            y += 22;
        }
    }

    /* Bottom divider of menu list */
    y += 4;
    DrawLine(MARGIN, y, MARGIN + 580, y, CLR_DIVIDER);
    y += 16;

    /* ---------------------------------------------------------------- */
    /*  Find section — Item ID textbox + FIND button                    */
    /* ---------------------------------------------------------------- */
    DrawText("Item ID:", MARGIN, y, FONT_BODY, CLR_TEXT);

    float idBoxH = 28.0f;
    float idBoxY = (float)y - 2;
    Rectangle idBox = { MARGIN + 80, idBoxY, 140, idBoxH };
    if (GuiTextBox(idBox, ctx->updateIdBuf, sizeof(ctx->updateIdBuf), ctx->updateIdEdit))
        ctx->updateIdEdit = !ctx->updateIdEdit;

    if (hasItems) {
        if (Btn((Rectangle){ MARGIN + 230, idBoxY, 80, idBoxH }, "FIND")) {
            int displayNum = atoi(ctx->updateIdBuf);
            if (displayNum < 1 || displayNum > count) {
                ctx->updateTarget = NULL;
                SetMessage(ctx, "No item with that ID.", CLR_ERROR);
            } else {
                ctx->updateTarget = menu_getItemByIndex(displayNum - 1);
                ctx->message[0] = '\0';
                ctx->updatePriceBuf[0] = '\0';
            }
        }
    } else {
        DrawText("(add items first)", MARGIN + 235, (int)idBoxY, FONT_SMALL, CLR_MUTED);
    }

    y = (int)(idBoxY + idBoxH + 20);

    /* ---------------------------------------------------------------- */
    /*  Selected Item section — only when an item has been found        */
    /* ---------------------------------------------------------------- */
    if (ctx->updateTarget) {
        /* Divider */
        DrawLine(MARGIN, y, MARGIN + 580, y, CLR_DIVIDER);
        y += 12;

        DrawText("Selected Item", MARGIN, y, FONT_HEAD, CLR_TEXT);
        y += 24;

        char buf[128];
        snprintf(buf, sizeof(buf), "Name: %s", ctx->updateTarget->itemName);
        DrawText(buf, MARGIN + 20, y, FONT_BODY, CLR_TEXT);
        y += 22;

        snprintf(buf, sizeof(buf), "Current price: $%.2f", (double)ctx->updateTarget->price);
        DrawText(buf, MARGIN + 20, y, FONT_BODY, CLR_TEXT);
        y += 22;

        snprintf(buf, sizeof(buf), "Current stock: %d", ctx->updateTarget->stock);
        DrawText(buf, MARGIN + 20, y, FONT_BODY, CLR_TEXT);
        y += 30;

        /* New price input */
        DrawText("New price ($):", MARGIN, y, FONT_BODY, CLR_TEXT);
        float priceBoxH = 28.0f;
        float priceBoxY = (float)y - 2;
        Rectangle priceBox = { MARGIN + 120, priceBoxY, 160, priceBoxH };
        if (GuiTextBox(priceBox, ctx->updatePriceBuf, sizeof(ctx->updatePriceBuf),
                       ctx->updatePriceEdit))
            ctx->updatePriceEdit = !ctx->updatePriceEdit;

        if (Btn((Rectangle){ MARGIN + 290, priceBoxY, 120, priceBoxH }, "SAVE PRICE")) {
            char *endptr;
            double price = strtod(ctx->updatePriceBuf, &endptr);
            if (endptr == ctx->updatePriceBuf || price <= 0.0 || price > 9999.99) {
                SetMessage(ctx, "Enter a valid price between 0.01 and 9999.99.", CLR_ERROR);
            } else {
                menu_updatePrice(ctx->updateTarget->itemID, (float)price);
                menu_save("data/menu.txt");
                SetMessage(ctx, "Item updated successfully.", CLR_SUCCESS);
                /* Refresh target via the same display number */
                int dn = atoi(ctx->updateIdBuf);
                ctx->updateTarget = menu_getItemByIndex(dn - 1);
            }
        }
    }

    /* ---------------------------------------------------------------- */
    /*  BACK button                                                     */
    /* ---------------------------------------------------------------- */
    if (Btn((Rectangle){ MARGIN, 580, 120, 36 }, "BACK"))
        GoTo(ctx, SCR_ADMIN_DASHBOARD);

    /* ---------------------------------------------------------------- */
    /*  Notification — bottom-right (via DrawNotification)               */
    /* ---------------------------------------------------------------- */
    DrawNotification(ctx, 0, 0);
}

/* ---------------------------------------------------------------- */
/*  Delete item                                                      */
/* ---------------------------------------------------------------- */
void DrawMenuDeleteScreen(GuiContext *ctx)
{
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int count = menu_getCount();
    bool hasItems = (count > 0);

    /* ---------------------------------------------------------------- */
    /*  Title — centred                                                 */
    /* ---------------------------------------------------------------- */
    const char *title = "DELETE MENU ITEM";
    int tw = MeasureText(title, FONT_TITLE);
    DrawText(title, (sw - tw) / 2, 25, FONT_TITLE, CLR_TITLE);

    /* ---------------------------------------------------------------- */
    /*  Available Menu Items list                                       */
    /* ---------------------------------------------------------------- */
    int y = 65;
    DrawText("Available Menu Items", MARGIN, y, FONT_HEAD, CLR_TEXT);
    y += 24;
    DrawLine(MARGIN, y, MARGIN + 580, y, CLR_DIVIDER);
    y += 8;

    if (!hasItems) {
        DrawText("No menu items available.", MARGIN, y, FONT_BODY, CLR_MUTED);
        y += 24;
    } else {
        for (int i = 0; i < count; i++) {
            const MenuItem *item = menu_getItemByIndex(i);
            char buf[128];
            snprintf(buf, sizeof(buf), "[%d]  %-28s  $%.2f",
                     i + 1, item->itemName, (double)item->price);
            DrawText(buf, MARGIN, y, FONT_BODY, CLR_TEXT);
            y += 22;
        }
    }

    /* Bottom divider of menu list */
    y += 4;
    DrawLine(MARGIN, y, MARGIN + 580, y, CLR_DIVIDER);
    y += 16;

    /* ---------------------------------------------------------------- */
    /*  Find section — Item ID textbox + FIND button                    */
    /* ---------------------------------------------------------------- */
    DrawText("Item ID:", MARGIN, y, FONT_BODY, CLR_TEXT);

    float idBoxH = 28.0f;
    float idBoxY = (float)y - 2;
    Rectangle idBox = { MARGIN + 80, idBoxY, 140, idBoxH };
    if (GuiTextBox(idBox, ctx->deleteIdBuf, sizeof(ctx->deleteIdBuf), ctx->deleteIdEdit))
        ctx->deleteIdEdit = !ctx->deleteIdEdit;

    if (hasItems) {
        if (Btn((Rectangle){ MARGIN + 230, idBoxY, 80, idBoxH }, "FIND")) {
            int displayNum = atoi(ctx->deleteIdBuf);
            if (displayNum < 1 || displayNum > count) {
                ctx->deleteTarget = NULL;
                SetMessage(ctx, "No item with that ID.", CLR_ERROR);
            } else {
                ctx->deleteTarget = menu_getItemByIndex(displayNum - 1);
                ctx->message[0] = '\0';
                ctx->deleteConfirming = false;
            }
        }
    } else {
        DrawText("(add items first)", MARGIN + 235, (int)idBoxY, FONT_SMALL, CLR_MUTED);
    }

    y = (int)(idBoxY + idBoxH + 20);

    /* ---------------------------------------------------------------- */
    /*  Selected Item section — only when item found, not confirming     */
    /* ---------------------------------------------------------------- */
    if (ctx->deleteTarget && !ctx->deleteConfirming) {
        DrawLine(MARGIN, y, MARGIN + 580, y, CLR_DIVIDER);
        y += 12;

        DrawText("Selected Item", MARGIN, y, FONT_HEAD, CLR_TEXT);
        y += 24;

        char buf[128];
        snprintf(buf, sizeof(buf), "Name: %s", ctx->deleteTarget->itemName);
        DrawText(buf, MARGIN + 20, y, FONT_BODY, CLR_TEXT);
        y += 22;

        snprintf(buf, sizeof(buf), "Current price: $%.2f",
                 (double)ctx->deleteTarget->price);
        DrawText(buf, MARGIN + 20, y, FONT_BODY, CLR_TEXT);
        y += 22;

        snprintf(buf, sizeof(buf), "Current stock: %d", ctx->deleteTarget->stock);
        DrawText(buf, MARGIN + 20, y, FONT_BODY, CLR_TEXT);
        y += 30;

        if (Btn((Rectangle){ MARGIN, y, 160, 38 }, "DELETE ITEM")) {
            ctx->deleteConfirming = true;
            ctx->message[0] = '\0';
        }
    }

    /* ---------------------------------------------------------------- */
    /*  Confirmation overlay                                            */
    /* ---------------------------------------------------------------- */
    if (ctx->deleteConfirming && ctx->deleteTarget) {
        /* Semi-transparent backdrop */
        DrawRectangle(0, 0, sw, sh, (Color){ 0, 0, 0, 160 });

        /* Confirmation panel */
        float panelW = 380.0f;
        float panelH = 200.0f;
        float px = (sw - panelW) / 2.0f;
        float py = (sh - panelH) / 2.0f;

        DrawRectangle(px, py, panelW, panelH, (Color){ 245, 245, 245, 255 });
        DrawRectangleLines(px, py, panelW, panelH, CLR_DIVIDER);

        /* Title */
        const char *confirmTitle = "Are you sure you want to delete this item?";
        int ct = MeasureText(confirmTitle, FONT_BODY);
        DrawText(confirmTitle, px + (panelW - ct) / 2.0f, py + 24, FONT_BODY, CLR_TEXT);

        /* Divider */
        DrawLine(px + 20, py + 55, px + panelW - 20, py + 55, CLR_DIVIDER);

        /* Item name — highlighted */
        int nt = MeasureText(ctx->deleteTarget->itemName, FONT_HEAD);
        DrawText(ctx->deleteTarget->itemName, px + (panelW - nt) / 2.0f,
                 py + 75, FONT_HEAD, CLR_TITLE);

        /* Buttons */
        float btnW = 130.0f;
        float btnH = 38.0f;
        float btnGap = 20.0f;
        float btnY = py + 130.0f;
        float totalW = btnW * 2 + btnGap;
        float btnStartX = px + (panelW - totalW) / 2.0f;

        if (Btn((Rectangle){ btnStartX, btnY, btnW, btnH }, "YES, DELETE")) {
            menu_deleteItem(ctx->deleteTarget->itemID);
            menu_save("data/menu.txt");
            SetMessage(ctx, "Item deleted successfully.", CLR_SUCCESS);
            ctx->deleteTarget = NULL;
            ctx->deleteConfirming = false;
            ctx->deleteIdBuf[0] = '\0';
        }
        if (Btn((Rectangle){ btnStartX + btnW + btnGap, btnY, btnW, btnH }, "CANCEL")) {
            ctx->deleteConfirming = false;
        }
    }

    /* ---------------------------------------------------------------- */
    /*  BACK button — hidden during confirmation                        */
    /* ---------------------------------------------------------------- */
    if (!ctx->deleteConfirming) {
        if (Btn((Rectangle){ MARGIN, 580, 120, 36 }, "BACK")) {
            ctx->deleteTarget = NULL;
            ctx->deleteConfirming = false;
            GoTo(ctx, SCR_ADMIN_DASHBOARD);
        }
    }

    /* ---------------------------------------------------------------- */
    /*  Notification — bottom-right (via DrawNotification)               */
    /* ---------------------------------------------------------------- */
    DrawNotification(ctx, 0, 0);
}
