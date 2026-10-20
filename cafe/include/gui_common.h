/* === include/gui_common.h ===
 * Shared GUI helper functions — buttons, notifications, table layout.
 */
#ifndef GUI_COMMON_H
#define GUI_COMMON_H

#include <stdbool.h>
#include "raylib.h"
#include "gui_state.h"

/* Simple button wrapper (fewer keystrokes at call sites) */
bool Btn(Rectangle r, const char *text);

/* Set a notification message to be displayed on the next frame */
void SetMessage(GuiContext *ctx, const char *text, Color color);

/* Draw the notification message if one is active */
void DrawNotification(GuiContext *ctx, int x, int y);

#endif /* GUI_COMMON_H */
