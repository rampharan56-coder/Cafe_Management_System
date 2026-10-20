/* === src/main_gui.c === */
/*
 * GUI-mode entry point for the Cafe Management System.
 * Loads data, then hands control to the Raylib front end.
 */
#include <stdio.h>
#include <stdlib.h>
#include "menu_logic.h"
#include "order_logic.h"
#include "fileio.h"
#include "gui.h"

/* ------------------------------------------------------------------ */
/*  Ensure the data/ directory exists (best-effort, portable)         */
/* ------------------------------------------------------------------ */
static void ensureDataDir(void)
{
#ifdef _WIN32
    system("if not exist data mkdir data");
#else
    system("mkdir -p data");
#endif
}

/* ------------------------------------------------------------------ */
/*  Entry point                                                        */
/* ------------------------------------------------------------------ */
int main(void)
{
    ensureDataDir();

    /* Menu: load from file; if absent, seed defaults and save */
    menu_load("data/menu.txt");
    if (menu_getCount() == 0) {
        menu_initDefaults();
        menu_save("data/menu.txt");
    }

    /* Orders: load existing history */
    order_load("data/orders.txt");

    /* Statistics: auto-reset monthly counters if month changed */
    menu_resetMonthlyStatsIfNeeded();

    /* Hand off to the Raylib front end */
    gui_run();

    return 0;
}
