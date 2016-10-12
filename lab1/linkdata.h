#ifndef LINKDATA_H_   /* Include guard */
#define LINKDATA_H_
#define MAX_SIZE 255
void setuplink(char* sport, int sportfd,int id);
int llopen(int fd, int id);
typedef struct { 

char* port; /*Dispositivo /dev/ttySx, x = 0, 1*/ 

int baudRate; /*Velocidade de transmissão*/ 

unsigned int sequenceNumber; /*Número de sequência da trama: 0, 1*/ 

unsigned int timeout; /*Valor do temporizador: 1 s*/ 

unsigned int numTransmissions; /*Número de tentativas em caso de falha*/ 

char frame[MAX_SIZE]; /*Trama*/ 

}  linklayer;
linklayer linkinfo;
int setupTermios(int fd);

#endif // LINKDATA_H_