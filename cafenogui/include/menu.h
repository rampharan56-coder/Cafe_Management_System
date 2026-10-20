/* === include/menu.h === */
#ifndef MENU_H
#define MENU_H

#define MAX_MENU_ITEMS 10
#define NAME_BUFFER    50

typedef struct {
    int itemID;
    char itemName[NAME_BUFFER];
    float price;
    int totalSold;    /* Lifetime sales — never resets */
    int monthlySold;  /* Monthly sales — resets automatically */
} MenuItem;

extern MenuItem menuItems[MAX_MENU_ITEMS];
extern int menuCount;
extern int nextMenuID;

void initializeMenu(void);
void viewMenu(void);
void viewMenuAdmin(void);
void addMenuItem(void);
void updateItem(void);
void deleteItem(void);
int findItemByID(int id);
int isValidName(const char *name);

#endif /* MENU_H */
