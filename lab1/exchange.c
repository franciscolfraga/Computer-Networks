#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "exchange.h"

serialport aplication;
linklayer protocol;
void setup(int id, char* file, char* sport){
	/* open the device to be non-blocking (read will return immediatly) */
    int filed = open(sport, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (filed <0) {perror(sport); exit(-1); }
    aplication.fd=filed;
    aplication.status=SENDER;
    protocol.port=sport;
    protocol.baudRate=4800;//change in main
    protocol.timeout=1;//change in main
    protocol.numTransmissions=3;//change in main opportunity to costumize
	
}