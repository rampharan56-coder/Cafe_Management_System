/* === include/gui_style.h ===
 * Centralised visual constants for the Raylib GUI.
 * All GUI screen files should pull dimensions and colours from here
 * rather than hardcoding magic numbers.
 */
#ifndef GUI_STYLE_H
#define GUI_STYLE_H

#include "raylib.h"

/* ---- Window ---- */
#define WIN_WIDTH   800
#define WIN_HEIGHT  660
#define WIN_TITLE   "Cafe Store"

/* ---- Layout ---- */
#define MARGIN      40
#define PANEL_GAP   12
#define BTN_WIDTH   220
#define BTN_HEIGHT  44

/* ---- Font sizes ---- */
#define FONT_TITLE  26
#define FONT_HEAD   20
#define FONT_BODY   16
#define FONT_SMALL  12

/* ---- Colours ---- */
#define CLR_BG          RAYWHITE
#define CLR_TITLE       (Color){ 90, 60, 30, 255 }   /* dark brown */
#define CLR_TEXT        BLACK
#define CLR_MUTED       GRAY
#define CLR_ACCENT      (Color){ 70, 130, 220, 255 } /* blue */
#define CLR_SUCCESS     DARKGREEN
#define CLR_ERROR       MAROON
#define CLR_LOW_STOCK   (Color){ 220, 80, 40, 255 }  /* rust red */
#define CLR_DIVIDER     LIGHTGRAY

#endif /* GUI_STYLE_H */
