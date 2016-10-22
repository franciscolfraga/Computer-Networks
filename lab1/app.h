#ifndef SETUP_H_   /* Include guard */
#define SETUP_H_


typedef struct{
int fd; /*Descritor correspondente à porta série*/ 
int status; /*TRANSMITTER | RECEIVER*/
FILE* file;
int file_size;
} sfile;
sfile appinfo;

void startapp(char* file_name);

void send(char* file_name);

void rcv();
int ID();
char* sFile();
char* rFile();
void printprofile(int id, char* file,char* sport);
void clearscreen();
char* serial();
char* searchcd(char* filename);
FILE* getOpen(char* path);

#define SENDER 1
#define RECEIVER 2
#endif // SETUP_H_