/* === src/fileio.c === */
/* File-persistence helpers that aren't owned by a specific logic module.
   Menu and order persistence live in their respective logic files. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileio.h"

/* ------------------------------------------------------------------ */
/*  Statistics persistence                                             */
/* ------------------------------------------------------------------ */

void saveStatistics(const char *filename, int month, int year)
{
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Warning: could not open '%s' for writing.\n", filename);
        return;
    }
    fprintf(fp, "Month=%d\n", month);
    fprintf(fp, "Year=%d\n", year);
    fclose(fp);
}

void loadStatistics(const char *filename, int *month, int *year)
{
    *month = -1;
    *year  = -1;

    FILE *fp = fopen(filename, "r");
    if (!fp)
        return;

    char line[128];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';

        char key[64], value[64];
        if (sscanf(line, "%63[^=]=%63s", key, value) == 2) {
            if (strcmp(key, "Month") == 0)
                *month = atoi(value);
            else if (strcmp(key, "Year") == 0)
                *year = atoi(value);
        }
    }

    fclose(fp);
}

/* ------------------------------------------------------------------ */
/*  Admin password persistence                                         */
/* ------------------------------------------------------------------ */

void saveAdminPassword(const char *filename, const char *password)
{
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Warning: could not open '%s' for writing.\n", filename);
        return;
    }
    fprintf(fp, "%s\n", password);
    fclose(fp);
}

void loadAdminPassword(const char *filename, char *stored, int size)
{
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        /* First run — create file with default password */
        strncpy(stored, "Adminhere", size - 1);
        stored[size - 1] = '\0';
        saveAdminPassword(filename, stored);
        return;
    }

    if (fgets(stored, size, fp)) {
        stored[strcspn(stored, "\n")] = '\0';
    } else {
        strncpy(stored, "Adminhere", size - 1);
        stored[size - 1] = '\0';
    }

    fclose(fp);
}
