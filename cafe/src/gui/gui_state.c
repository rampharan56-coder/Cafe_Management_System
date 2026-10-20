/* === src/gui/gui_state.c ===
 * Screen navigation and shared GUI state.
 */
#include "gui_state.h"
#include <string.h>

void GoTo(GuiContext *ctx, Screen s)
{
    ctx->currentScreen = s;
    ctx->message[0] = '\0';
}

void InitGuiContext(GuiContext *ctx)
{
    memset(ctx, 0, sizeof(*ctx));
    ctx->currentScreen = SCR_WELCOME;
    ctx->adminAttemptsLeft = 3;
    ctx->analyticsMonthly = 1;
    ctx->historyIndex = 0;
    for (int i = 0; i < MAX_MENU_ITEMS; i++)
        ctx->cartQty[i] = 1;
}
