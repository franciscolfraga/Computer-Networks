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

#define FRAME_SIZE 1024
#define DATA_FRAME_SIZE 6

typedef struct { 

char* port; /*Dispositivo /dev/ttySx, x = 0, 1*/ 

int baudRate; /*Velocidade de transmissão*/ 

unsigned int sequenceNumber; /*Número de sequência da trama: 0, 1*/ 

unsigned int timeout; /*Valor do temporizador: 1 s*/ 

unsigned int numTransmissions; /*Número de tentativas em caso de falha*/ 

char frame[FRAME_SIZE]; /*Trama*/ 

unsigned int sn;

}  linklayer;


typedef struct {
	unsigned char frame[FRAME_SIZE];
	unsigned int size;
	unsigned int sn;
	unsigned int frametype;
} Frame;
typedef enum { false, true } bool;
bool received;
linklayer linkinfo;
extern Frame reader;
void setuplink(char* sport,int id);

int llopen(int fd, int id);

int rcvmachine(int fd , int id);

int setupTermios(int fd);

unsigned char getA(int type,int id);

int sendcmd(int fd, int frametype, int id);

unsigned char callRead(int fd);

int openport(char* sport);

int closeport(int fd);

int llwrite(int fd, unsigned char* buffer, int length);

int llread(int fd, unsigned char* buffer);

int sendDataFrame(int fd, unsigned char* data, unsigned int size);

unsigned char getBCC2(unsigned char* data, unsigned int size);

int checkcmd(unsigned char cmd);

Frame stuff(Frame df);

Frame destuff(Frame df);

#endif // LINKDATA_H_