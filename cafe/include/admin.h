/* === include/admin.h === */
#ifndef ADMIN_H
#define ADMIN_H

#include <stdbool.h>

#define PW_BUFFER 64

/* Pure password check — no console I/O. Safe to call from any UI layer. */
bool admin_verifyPassword(const char *entered);

/* Console-UI login with 3-attempt loop */
bool adminLogin(void);

/* Console-UI admin menu loop */
void adminMenu(void);

#endif /* ADMIN_H */
