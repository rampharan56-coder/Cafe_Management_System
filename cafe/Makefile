# Makefile for Cafe Management System
#
# Targets:
#   make console    — Build the text-mode console version (no external deps)
#   make gui        — Build the Raylib GUI version (requires raylib + raygui)
#   make all        — Build both
#   make clean      — Remove build/ directory
#
# Both binaries reuse the same business logic (menu_logic, order_logic,
# fileio, utils). Only the UI layer and entry point differ.
#
# === Windows MSYS2 UCRT64 ===  (primary target)
#   Run from the MSYS2 UCRT64 terminal (ucrt64.exe).
#   The UCRT64 environment's gcc, headers, and libraries are found
#   automatically because pacman installs into the active environment.
#
#   Prerequisites (one-time):
#     pacman -S mingw-w64-ucrt-x86_64-raylib
#     Download raygui.h into the project's include/ folder from:
#       https://raw.githubusercontent.com/raysan5/raygui/master/src/raygui.h
#
# === Linux / WSL ===
#   make console works out of the box.
#   make gui requires raylib development libraries for your distro.

CC       = gcc
CFLAGS   = -Wall -Wextra -pedantic -std=c11 -Iinclude
LDFLAGS  =
BUILD    = build
OBJDIR   = $(BUILD)/obj

# ----- Platform detection -----
# MSYS2 UCRT64's gcc targets Windows and defines _WIN32.
# It also inherits OS=Windows_NT from the Windows kernel.
ifeq ($(OS),Windows_NT)
  # Windows / MSYS2 UCRT64
  CONSOLE_BIN = $(BUILD)/cafe_console.exe
  GUI_BIN     = $(BUILD)/cafe_gui.exe
  # UCRT64 raylib is installed via pacman -S mingw-w64-ucrt-x86_64-raylib
  GUI_LIBS    = -lraylib -lopengl32 -lgdi32 -lwinmm
else
  # Linux / WSL
  CONSOLE_BIN = $(BUILD)/cafe_console
  GUI_BIN     = $(BUILD)/cafe_gui
  GUI_LIBS    = -lraylib -lm -lGL -lpthread -ldl \
                -lX11 -lXrandr -lXinerama -lXcursor -lXi -lXxf86vm
endif

# ----- Shared backend objects -----
BACKEND_SRCS = src/admin.c src/menu_logic.c src/order_logic.c src/fileio.c src/utils.c
BACKEND_OBJS = $(BACKEND_SRCS:src/%.c=$(OBJDIR)/%.o)

# ----- Console UI objects -----
CONSOLE_SRCS  = src/main_console.c src/console/menu_console.c \
                src/console/order_console.c src/console/admin_console.c
CONSOLE_OBJS  = $(CONSOLE_SRCS:src/%.c=$(OBJDIR)/%.o)

# ----- GUI objects (requires raylib + raygui) -----
GUI_SRCS  = src/main_gui.c src/gui/gui_main.c src/gui/gui_state.c \
            src/gui/gui_common.c src/gui/gui_login.c src/gui/gui_admin.c \
            src/gui/gui_menu.c src/gui/gui_customer.c src/gui/gui_order.c \
            src/gui/gui_analytics.c src/gui/gui_inventory.c
GUI_OBJS  = $(GUI_SRCS:src/%.c=$(OBJDIR)/%.o)

# ----- Rules -----

.PHONY: all console gui clean

all: console gui

console: $(CONSOLE_BIN)

gui: $(GUI_BIN)

# Link console binary
$(CONSOLE_BIN): $(BACKEND_OBJS) $(CONSOLE_OBJS)
	@mkdir -p $(BUILD)
	$(CC) $^ -o $@ $(LDFLAGS)

# Link GUI binary
$(GUI_BIN): $(BACKEND_OBJS) $(GUI_OBJS)
	@mkdir -p $(BUILD)
	$(CC) $^ -o $@ $(LDFLAGS) $(GUI_LIBS)

# Compile rule: maps src/X.c -> build/obj/X.o
$(OBJDIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD)
