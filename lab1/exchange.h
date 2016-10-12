#ifndef EXCHANGE_H_   /* Include guard */
#define EXCHANGE_H_


typedef struct{
int fd; /*Descritor correspondente à porta série*/ 
int status; /*TRANSMITTER | RECEIVER*/ 
} serialport;



typedef struct{ 
char* port; /*Dispositivo /dev/ttySx, x = 0, 1*/ 
int baudRate; /*Velocidade de transmissão*/ 
unsigned int sequenceNumber; /*Número de sequência da trama: 0, 1*/ 
unsigned int timeout; /*Valor do temporizador: 1 s*/ 
unsigned int numTransmissions; /*Número de tentativas em caso de falha*/ 
char frame[255]; /*Trama*/ 
} linklayer;



void setup(int id, char* file, char* sport);


#define SENDER 1
#define RECEIVER 2
#endif // EXCHANGE_H_