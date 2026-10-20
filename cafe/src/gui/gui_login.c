/* === src/gui/gui_login.c ===
 * Admin login screen — centered panel with clean aligned layout.
 */
#include <string.h>
#include <stdio.h>
#include "raylib.h"
#include "raygui.h"
#include "gui_state.h"
#include "gui_style.h"
#include "gui_common.h"

void DrawAdminLoginScreen(GuiContext *ctx)
{
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    /* ---------------------------------------------------------------- */
    /*  Login panel — centred in the window                             */
    /* ---------------------------------------------------------------- */
    float panelW = 360.0f;
    float panelH = 250.0f;
    float px = (sw - panelW) / 2.0f;
    float py = (sh - panelH) / 2.0f;

    /* Light panel background with subtle border */
    DrawRectangle(px, py, panelW, panelH, (Color){ 245, 245, 245, 255 });
    DrawRectangleLines(px, py, panelW, panelH, CLR_DIVIDER);

    /* ---------------------------------------------------------------- */
    /*  Title — centred at panel top                                    */
    /* ---------------------------------------------------------------- */
    const char *title = "ADMIN LOGIN";
    int tw = MeasureText(title, FONT_TITLE);
    DrawText(title, px + (panelW - tw) / 2.0f, py + 20, FONT_TITLE, CLR_TITLE);

    /* Divider line */
    DrawLine(px + 20, py + 55, px + panelW - 20, py + 55, CLR_DIVIDER);

    /* ---------------------------------------------------------------- */
    /*  Password label                                                  */
    /* ---------------------------------------------------------------- */
    float labelY = py + 78;
    DrawText("Password:", px + 40, labelY, FONT_BODY, CLR_TEXT);

    /* ---------------------------------------------------------------- */
    /*  Text box — centred within the panel                             */
    /* ---------------------------------------------------------------- */
    float tbW = 280.0f;
    float tbX = px + (panelW - tbW) / 2.0f;
    float tbY = labelY + 26;
    Rectangle box = { tbX, tbY, tbW, 32 };
    if (GuiTextBox(box, ctx->adminPasswordBuf, PW_BUFFER, ctx->adminPwEditMode))
        ctx->adminPwEditMode = !ctx->adminPwEditMode;

    /* ---------------------------------------------------------------- */
    /*  LOGIN / BACK buttons — side by side, equal width, centred       */
    /* ---------------------------------------------------------------- */
    float btnW = 120.0f;
    float btnH = 36.0f;
    float btnGap = 16.0f;
    float btnY = tbY + 52;
    float totalBtnsW = btnW * 2 + btnGap;
    float btnStartX = px + (panelW - totalBtnsW) / 2.0f;

    if (Btn((Rectangle){ btnStartX, btnY, btnW, btnH }, "LOGIN")) {
        if (admin_verifyPassword(ctx->adminPasswordBuf)) {
            ctx->adminPasswordBuf[0] = '\0';
            SetMessage(ctx, "Login successful.", CLR_SUCCESS);
            GoTo(ctx, SCR_ADMIN_DASHBOARD);
        } else {
            ctx->adminAttemptsLeft--;
            ctx->adminPasswordBuf[0] = '\0';
            if (ctx->adminAttemptsLeft <= 0) {
                SetMessage(ctx, "Too many failed attempts.", CLR_ERROR);
                GoTo(ctx, SCR_WELCOME);
            } else {
                char buf[64];
                snprintf(buf, sizeof(buf), "Wrong password. %d attempt(s) left.",
                         ctx->adminAttemptsLeft);
                SetMessage(ctx, buf, CLR_ERROR);
            }
        }
    }
    if (Btn((Rectangle){ btnStartX + btnW + btnGap, btnY, btnW, btnH }, "BACK"))
        GoTo(ctx, SCR_WELCOME);

    /* ---------------------------------------------------------------- */
    /*  Notification — centred below the panel                          */
    /* ---------------------------------------------------------------- */
    if (ctx->message[0]) {
        int mw = MeasureText(ctx->message, FONT_BODY);
        DrawText(ctx->message, (sw - mw) / 2, py + panelH + 20, FONT_BODY,
                 ctx->messageColor);
    }
}
