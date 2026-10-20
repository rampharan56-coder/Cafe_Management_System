/* === src/admin.c ===
 * Pure authentication logic — no console I/O.
 * The console UI functions (adminLogin, adminMenu) live in
 * src/console/admin_console.c so they can stay with their UI layer.
 */
#include <string.h>
#include "admin.h"
#include "fileio.h"

bool admin_verifyPassword(const char *entered)
{
    char storedPassword[PW_BUFFER];
    loadAdminPassword("data/admin.txt", storedPassword, PW_BUFFER);
    return strcmp(entered, storedPassword) == 0;
}
