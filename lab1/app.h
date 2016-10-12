#ifndef SETUP_H_   /* Include guard */
#define SETUP_H_


typedef struct{
int fd; /*Descritor correspondente à porta série*/ 
int status; /*TRANSMITTER | RECEIVER*/
FILE* file;
} sfile;
sfile appinfo;

char* searchcd(char* filename);
void setupapp(int id, char* file, char* sport);
FILE* getOpen(char* path);

#define SENDER 1
#define RECEIVER 2
#endif // SETUP_H_