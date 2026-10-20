/* === include/fileio.h === */
#ifndef FILEIO_H
#define FILEIO_H

/* Statistics persistence (used by menu_logic) */
void saveStatistics(const char *filename, int month, int year);
void loadStatistics(const char *filename, int *month, int *year);

/* Admin password persistence */
void saveAdminPassword(const char *filename, const char *password);
void loadAdminPassword(const char *filename, char *stored, int size);

#endif /* FILEIO_H */
