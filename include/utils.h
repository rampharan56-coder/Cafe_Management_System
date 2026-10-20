/* === include/utils.h === */
#ifndef UTILS_H
#define UTILS_H

#ifdef _WIN32
  #define CLEAR_SCREEN() system("cls")
#else
  #define CLEAR_SCREEN() printf("\033[H\033[J")
#endif

void waitKey(const char *prompt);
int  readInt(const char *prompt, int min, int max);
float readFloat(const char *prompt, float min, float max);
void readString(const char *prompt, char *buf, int size);


int  readYesNo(const char *prompt);

#endif /* UTILS_H */
