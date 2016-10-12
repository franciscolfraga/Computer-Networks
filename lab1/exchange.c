#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "exchange.h"

void letsExchange(int id, char* file, char* sport){
	serialport ss;
	/* open the device to be non-blocking (read will return immediatly) */
    int filed = open(sport, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (filed <0) {perror(sport); exit(-1); }
    ss.fd=filed;
    ss.status=SENDER;
	printf("I'm Here and opened the port the fd is %d and the status is %d\n",ss.fd,ss.status);
}