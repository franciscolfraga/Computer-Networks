#ifndef EXCHANGE_H_   /* Include guard */
#define EXCHANGE_H_
typedef struct{
int fd; /*Descritor correspondente à porta série*/ 
int status; /*TRANSMITTER | RECEIVER*/ 
} serialport;
void letsExchange(int id, char* file, char* sport);
#define SENDER 1
#define RECEIVER 2
#endif // EXCHANGE_H_