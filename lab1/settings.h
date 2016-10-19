#ifndef SETTINGS_H_   /* Include guard */
#define SETTINGS_H_

int ID();
char* sFile();
char* rFile();
void printprofile(int id, char* file,char* sport);
void clearscreen();
char* serial();
char* searchcd(char* filename);
FILE* getOpen(char* path);

#endif // SETTINGS_H_