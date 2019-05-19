#ifndef util_h
#define util_h

#include <stdio.h>
#define BUFLEN 256

char* yesorno(int condition);
FILE* openfile(const char* filename, const char* openflags);

void printleft(const char* left);
void printright(const char* right);
void printboth(const char* left_right);
void iprintright(const char* right);
void cprintleft(const char* left);
void cprintright(const char* right);
void cprintboth(const char* left_right);

void printline(void);

#endif /* util_h */
