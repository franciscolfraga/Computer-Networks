#ifndef LINKDATA_H_   /* Include guard */
#define LINKDATA_H_
#define MAX_SIZE 255
#define SENDER 1
#define RECEIVER 2

#define CSET 0x03
#define CUA 0x07
#define CRR 0x05
#define CREJ 0x01
#define CDISC 0x0B

#define FLAG 0x7E
#define A03 0x03
#define A01 0x01
#define ESCAPE 0x7D

#define FRAME_SIZE 5

void setuplink(char* sport, int sportfd,int id);

int llopen(int fd, int id);

typedef struct { 

char* port; /*Dispositivo /dev/ttySx, x = 0, 1*/ 

int baudRate; /*Velocidade de transmissão*/ 

unsigned int sequenceNumber; /*Número de sequência da trama: 0, 1*/ 

unsigned int timeout; /*Valor do temporizador: 1 s*/ 

unsigned int numTransmissions; /*Número de tentativas em caso de falha*/ 

char frame[FRAME_SIZE]; /*Trama*/ 

}  linklayer;


typedef struct {
	unsigned char frame[FRAME_SIZE];
	unsigned int size;
	unsigned int sn;
	unsigned int frametype;
} Frame;

linklayer linkinfo;

int recmachine(int fd , int id);

int setupTermios(int fd);

unsigned char getA(int type,int id);

int sendcmd(int fd, int frametype, int id);

#endif // LINKDATA_H_