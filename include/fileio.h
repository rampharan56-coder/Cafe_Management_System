/* === include/fileio.h === */
#ifndef FILEIO_H
#define FILEIO_H

void saveMenu(const char *filename);
void loadMenu(const char *filename);
void saveOrders(const char *filename);
void loadOrders(const char *filename);
void saveAdminPassword(const char *filename, const char *password);
void loadAdminPassword(const char *filename, char *stored, int size);

/* --- Statistics persistence --- */
void saveStatistics(const char *filename, int month, int year);
void loadStatistics(const char *filename, int *month, int *year);

/* --- Ingredient persistence --- */
void saveIngredients(const char *filename);
void loadIngredients(const char *filename);

/* --- Recipe persistence --- */
void saveRecipes(const char *filename);
void loadRecipes(const char *filename);

/* --- Finance persistence --- */
void saveFinance(const char *filename);
void loadFinance(const char *filename);

/* --- Discount persistence --- */
void saveDiscounts(const char *filename);
void loadDiscounts(const char *filename);

#endif /* FILEIO_H */
