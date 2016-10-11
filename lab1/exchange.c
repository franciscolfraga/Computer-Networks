#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


void letsExchange(int id, char* sport, char* file){
	/* open the device to be non-blocking (read will return immediatly) */
    int fd = open(sport, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd <0) {perror(sport); exit(-1); }

	printf("I'm Here and opened the port\n");
}