/* === src/gui/gui_common.c ===
 * Shared GUI helpers — buttons, notifications, common layouts.
 * Every function operates on a GuiContext passed by pointer.
 *
 * RAYGUI_IMPLEMENTATION is defined HERE — exactly one .c file
 * so the raygui function bodies are compiled once.
 */
#define RAYGUI_IMPLEMENTATION
#include <string.h>
#include "raylib.h"
#include "raygui.h"
#include "gui_state.h"
#include "gui_style.h"

/* ---------------------------------------------------------------- */
/*  Button wrapper — fewer keystrokes at call sites                 */
/* ---------------------------------------------------------------- */
bool Btn(Rectangle r, const char *text)
{
    return GuiButton(r, text);
}

/* ---------------------------------------------------------------- */
/*  Notification helpers                                             */
/* ---------------------------------------------------------------- */
void SetMessage(GuiContext *ctx, const char *text, Color color)
{
    strncpy(ctx->message, text, sizeof(ctx->message) - 1);
    ctx->message[sizeof(ctx->message) - 1] = '\0';
    ctx->messageColor = color;
}

/* ---------------------------------------------------------------- */
/*  Draw a notification line at bottom-right of screen               */
/* ---------------------------------------------------------------- */
void DrawNotification(GuiContext *ctx, int x, int y)
{
    (void)x;
    (void)y;
    if (ctx->message[0]) {
        int mw = MeasureText(ctx->message, FONT_BODY);
        DrawText(ctx->message,
                 GetScreenWidth() - mw - MARGIN,
                 GetScreenHeight() - MARGIN - FONT_BODY - 4,
                 FONT_BODY,
                 ctx->messageColor);
    }
}

/* ---------------------------------------------------------------- */
/*  Table header helper — draws grey column labels + divider        */
/* ---------------------------------------------------------------- */
void DrawTableHeader(int x, int *y, const char *cols[], int colX[], int colCount)
{
    for (int i = 0; i < colCount; i++)
        DrawText(cols[i], x + colX[i], *y, FONT_SMALL, CLR_MUTED);
    *y += 22;
    DrawLine(x, *y, x + 680, *y, CLR_DIVIDER);
    *y += 8;
}
