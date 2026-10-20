/* === include/gui_state.h ===
 * GUI screen navigation and shared state.
 *
 * All mutable UI state lives in a single GuiContext struct.
 * Every screen function receives a pointer to it so the GUI layer
 * has no module-level globals — easier to test, easier to reason about.
 */
#ifndef GUI_STATE_H
#define GUI_STATE_H

#include <stdbool.h>
#include "raylib.h"
#include "menu_logic.h"
#include "order_logic.h"
#include "admin.h"

/* ================================================================ */
/*  Screens                                                          */
/* ================================================================ */

typedef enum {
    SCR_WELCOME,
    SCR_ADMIN_LOGIN,
    SCR_ADMIN_DASHBOARD,
    SCR_MENU_VIEW,
    SCR_MENU_ADD,
    SCR_MENU_UPDATE,
    SCR_MENU_DELETE,
    SCR_ORDER_HISTORY,
    SCR_ANALYTICS,
    SCR_CUSTOMER_MENU,
    SCR_RECEIPT,
    SCR_INVENTORY_VIEW,
    SCR_INVENTORY_RESTOCK
} Screen;

/* ================================================================ */
/*  Context                                                          */
/* ================================================================ */

typedef struct {
    /* ---- Navigation ---- */
    Screen currentScreen;
    bool   exitRequested;

    /* ---- Notification bar ---- */
    char  message[128];
    Color messageColor;

    /* ---- Admin login ---- */
    char  adminPasswordBuf[PW_BUFFER];
    bool  adminPwEditMode;
    int   adminAttemptsLeft;

    /* ---- Add-item form ---- */
    char  addNameBuf[NAME_BUFFER];
    char  addPriceBuf[16];
    bool  addNameEdit;
    bool  addPriceEdit;

    /* ---- Update-item form ---- */
    char  updateIdBuf[8];
    char  updatePriceBuf[16];
    bool  updateIdEdit;
    bool  updatePriceEdit;
    const MenuItem *updateTarget;

    /* ---- Delete-item form ---- */
    char  deleteIdBuf[8];
    bool  deleteIdEdit;
    const MenuItem *deleteTarget;
    bool  deleteConfirming;     /* true = showing confirmation overlay */

    /* ---- Order history ---- */
    int   historyIndex;

    /* ---- Analytics ---- */
    int   analyticsMonthly;   /* 1 = monthly, 0 = lifetime */

    /* ---- Customer ordering ---- */
    Order cart;
    int   cartQty[MAX_MENU_ITEMS];
    Order lastOrder;
    bool  haveLastOrder;

    /* ---- Inventory restock ---- */
    int   invRestockDisplayNum;
} GuiContext;

/* ================================================================ */
/*  Navigation helpers                                               */
/* ================================================================ */

/* Navigate to a screen, clearing the notification message. */
void GoTo(GuiContext *ctx, Screen s);

/* Initialise a fresh GuiContext (call once at startup). */
void InitGuiContext(GuiContext *ctx);

#endif /* GUI_STATE_H */
