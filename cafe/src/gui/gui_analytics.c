/* === src/gui/gui_analytics.c ===
 * Sales analytics screen — monthly and lifetime rankings with bar chart.
 */
#include <string.h>
#include <stdio.h>
#include "raylib.h"
#include "raygui.h"
#include "gui_state.h"
#include "gui_style.h"
#include "gui_common.h"

/* ---------------------------------------------------------------- */
/*  Bar chart helper (self-contained, no external dependencies)      */
/* ---------------------------------------------------------------- */
static void DrawBarChart(Rectangle bounds, const char **labels, const int *values,
                          int count, const char *title)
{
    DrawRectangleLinesEx(bounds, 1, CLR_MUTED);
    DrawText(title, (int)bounds.x, (int)bounds.y - 24, FONT_BODY, CLR_TEXT);

    if (count <= 0) {
        DrawText("No sales data yet.", (int)bounds.x + 12, (int)bounds.y + 12,
                 FONT_BODY, CLR_MUTED);
        return;
    }

    int maxVal = 1;
    for (int i = 0; i < count; i++)
        if (values[i] > maxVal) maxVal = values[i];

    const float padding  = 12.0f;
    const float labelArea = 22.0f;
    float chartW = bounds.width  - 2 * padding;
    float chartH = bounds.height - 2 * padding - labelArea;
    float barGap = 8.0f;
    float barWidth = (chartW - barGap * (count - 1)) / count;
    if (barWidth < 6) barWidth = 6;

    for (int i = 0; i < count; i++) {
        float barH = ((float)values[i] / (float)maxVal) * chartH;
        if (barH < 1 && values[i] > 0) barH = 1;

        float x = bounds.x + padding + i * (barWidth + barGap);
        float y = bounds.y + padding + (chartH - barH);

        DrawRectangle((int)x, (int)y, (int)barWidth, (int)barH, CLR_ACCENT);
        DrawRectangleLines((int)x, (int)y, (int)barWidth, (int)barH,
                           (Color){30, 70, 140, 255});

        char valBuf[16];
        snprintf(valBuf, sizeof(valBuf), "%d", values[i]);
        int tw = MeasureText(valBuf, FONT_SMALL);
        DrawText(valBuf, (int)(x + barWidth / 2 - tw / 2), (int)y - 16,
                 FONT_SMALL, CLR_TEXT);

        char lbl[10];
        snprintf(lbl, sizeof(lbl), "%.9s", labels[i]);
        int lw = MeasureText(lbl, 10);
        DrawText(lbl, (int)(x + barWidth / 2 - lw / 2),
                 (int)(bounds.y + padding + chartH + 6), 10, CLR_TEXT);
    }
}

/* ---------------------------------------------------------------- */
/*  Analytics screen                                                 */
/* ---------------------------------------------------------------- */
void DrawAnalyticsScreen(GuiContext *ctx)
{
    DrawText("SALES ANALYTICS", MARGIN, 30, FONT_TITLE, CLR_TEXT);

    if (Btn((Rectangle){MARGIN, 80, 150, 36},
            ctx->analyticsMonthly ? "[ Monthly ]" : "Monthly"))
        ctx->analyticsMonthly = 1;
    if (Btn((Rectangle){200, 80, 150, 36},
            !ctx->analyticsMonthly ? "[ Lifetime ]" : "Lifetime"))
        ctx->analyticsMonthly = 0;

    int count = menu_getCount();

    if (count == 0) {
        DrawText("No menu items to analyze.", MARGIN, 140, FONT_BODY, CLR_MUTED);
    } else {
        int indices[MAX_MENU_ITEMS];
        menu_getSalesSortedIndices(indices, ctx->analyticsMonthly);

        const char *labels[MAX_MENU_ITEMS];
        int   values[MAX_MENU_ITEMS];
        for (int i = 0; i < count; i++) {
            const MenuItem *item = menu_getItemByIndex(indices[i]);
            labels[i] = item->itemName;
            values[i] = ctx->analyticsMonthly ? item->monthlySold : item->totalSold;
        }

        Rectangle chartBounds = {60, 150, 680, 320};
        DrawBarChart(chartBounds, labels, values, count,
                     ctx->analyticsMonthly
                         ? "Monthly Sales Ranking"
                         : "Lifetime Sales Ranking");

        int bestIdx = ctx->analyticsMonthly
                          ? menu_getMonthlyBestSeller()
                          : menu_getLifetimeBestSeller();
        if (bestIdx != -1) {
            const MenuItem *best = menu_getItemByIndex(bestIdx);
            int bestVal = ctx->analyticsMonthly ? best->monthlySold : best->totalSold;
            char buf[160];
            snprintf(buf, sizeof(buf), "Best seller (%s): %s -- %d cups",
                     ctx->analyticsMonthly ? "this month" : "all time",
                     best->itemName, bestVal);
            DrawText(buf, 60, 500, FONT_BODY, CLR_SUCCESS);
        }
    }

    if (Btn((Rectangle){MARGIN, 560, 120, 36}, "BACK")) GoTo(ctx, SCR_ADMIN_DASHBOARD);
}
