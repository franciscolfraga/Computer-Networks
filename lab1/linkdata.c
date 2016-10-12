#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <strings.h>

#include "linkdata.h"
struct termios oldtio,newtio;
void setuplink(char* sport, int sportfd,int id){
	//new termios
	printf("\tSetting up data link layer info...\n");
	if(setupTermios(sportfd)<0){
		printf("\n\tError on setup of termios!\n");
		exit(-1);
	}


	//setup da struct
	linkinfo.port=sport;
	linkinfo.baudRate=4800; //default minha (depois costumizar)
	linkinfo.sequenceNumber=0;
	linkinfo.timeout=1;
	linkinfo.numTransmissions=3;
	printf("\tAll set in data link layer info!\n");
	if(llopen(sportfd,id)<0){
		printf("\n\tError setting up llopen!\n");
	}

}
int setupTermios(int fd){
	//save old serial port settings, add control later
	printf("\tSaving current settings of termios...\n");
	tcgetattr(fd, &oldtio);
	printf("\tOld termios settings saved!\n");
	//setup of new Termios (asynchronous as asked)
	printf("\tSetting up new termios...\n");
	bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = linkinfo.baudRate | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR | ICRNL;
    newtio.c_oflag = 0;
    newtio.c_lflag = ICANON;
    newtio.c_cc[VMIN]=1;
    newtio.c_cc[VTIME]=0;
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);
    printf("\tNew termios setup done!\n");
    //add control later
    return 1;
}
int llopen(int fd, int id){
	return 1;
}