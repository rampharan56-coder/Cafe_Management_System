/* === src/utils.c === */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "utils.h"

/* Flush any leftover characters from stdin up to and including \n */
static void flushStdin(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

static void flushIfTruncated(const char *buf, int size)
{
    /* fgets stores the \n if the line fit; if it's absent the line was cut */
    size_t len = strcspn(buf, "\n");
    if (buf[len] != '\n' && len == (size_t)size - 1) {
        flushStdin();
    }
}



void waitKey(const char *prompt)
{
    if (prompt && prompt[0] != '\0')
        printf("%s", prompt);
    else
        printf("Press Enter to continue...");
    fflush(stdout);
    flushStdin();
}



int readInt(const char *prompt, int min, int max)
{
    int  value;
    char line[64];

    while (1) {
        printf("%s", prompt);
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n  Input closed. Exiting.\n");
            exit(0);
        }

        /* Strip trailing newline */
        line[strcspn(line, "\n")] = '\0';

        char *endptr;
        long  v = strtol(line, &endptr, 10);

        if (endptr == line || *endptr != '\0') {
            printf("  Invalid input. Please enter a number");
            if (min != max)
                printf(" between %d and %d", min, max);
            printf(".\n");
            flushIfTruncated(line, (int)sizeof(line));
            continue;
        }

        if (v < (long)min || v > (long)max) {
            printf("  Out of range. Enter a value between %d and %d.\n", min, max);
            continue;
        }

        value = (int)v;
        break;
    }
    return value;
}


float readFloat(const char *prompt, float min, float max)
{
    float value;
    char  line[64];

    while (1) {
        printf("%s", prompt);
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n  Input closed. Exiting.\n");
            exit(0);
        }

        line[strcspn(line, "\n")] = '\0';

        char *endptr;
        double v = strtod(line, &endptr);

        if (endptr == line || *endptr != '\0') {
            printf("  Invalid input. Please enter a number");
            if (min != max)
                printf(" between %.2f and %.2f", (double)min, (double)max);
            printf(".\n");
            flushIfTruncated(line, (int)sizeof(line));
            continue;
        }

        if ((double)v < (double)min || (double)v > (double)max) {
            printf("  Out of range. Enter a value between %.2f and %.2f.\n",
                   (double)min, (double)max);
            continue;
        }

        value = (float)v;
        break;
    }
    return value;
}


void readString(const char *prompt, char *buf, int size)
{
    while (1) {
        printf("%s", prompt);
        fflush(stdout);

        if (!fgets(buf, size, stdin)) {
            printf("\n  Input closed. Exiting.\n");
            exit(0);
        }

        /* Strip trailing newline */
        size_t len = strcspn(buf, "\n");
        buf[len] = '\0';

        if (len == (size_t)size - 1) {
            flushStdin();
        }

        if (buf[0] != '\0')
            break;

        printf("  Input cannot be empty.\n");
    }
}


int readYesNo(const char *prompt)
{

    char line[16];

    while (1) {
        printf("%s", prompt);
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin))
            return -1;

        size_t len = strcspn(line, "\n");
        line[len] = '\0';

        /* Flush leftover if truncated */
        if (len == sizeof(line) - 1)
            flushStdin();

        if (line[0] == 'Y' || line[0] == 'y')
            return 1;

        if (line[0] == 'N' || line[0] == 'n' || line[0] == '\0')
            return 0;

        printf("  Please enter Y or N.\n");
    }
}
