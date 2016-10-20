#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <string.h>
#include <strings.h>

#include "linkdata.h"
#include "alarm.h"
#include "app.h"
struct termios oldtio,newtio;
Frame reader;


void setuplink(char* sport,int id){
	//new termios
	int sportfd;
	sportfd=openport(sport);
	printf("\tSetting up data link layer info...\n");
	if(setupTermios(sportfd)<0){
		printf("\n\tError on setup of termios!\n");
		exit(-1);
	}
	//ver boas práticas e mudar
	//setup da struct
	linkinfo.port = sport;
	linkinfo.baudRate = B4800; //default minha (depois costumizar)
	linkinfo.sequenceNumber = 0;
	linkinfo.timeout = 3;
	linkinfo.sn=0;
	linkinfo.numTransmissions = 3;
	printf("\tAll set in data link layer info!\n");
	if(llopen(sportfd , id)!=sportfd){
		printf("\n\tError setting up llopen! Bad file descriptor received\n");
		exit(-1);
	}

}


int setupTermios(int fd){
	//save old serial port settings, add control later
	printf("\tSaving current settings of termios...\n");
	tcgetattr(fd , &oldtio);
	printf("\tOld termios settings saved!\n");
	//setup of new Termios (asynchronous as asked)
	printf("\tSetting up new termios...\n");
	bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = linkinfo.baudRate | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR | ICRNL;
    newtio.c_oflag = 0;
    newtio.c_lflag = ICANON;
    newtio.c_cc[VMIN] = 1;
    newtio.c_cc[VTIME] = 0.5;
    tcflush(fd , TCIFLUSH);
    tcsetattr(fd , TCSANOW,&newtio);
    printf("\tNew termios setup done!\n");
    //add control later
    return 1;
}


int llopen(int fd , int id){
	int counter = 0;
	received=false;
	printf("\tSetting up connection...\n");
		if(id == SENDER){
			while(counter < linkinfo.numTransmissions && received==false){
				if(	buzz==1 || counter == 0){
					buzz=0;
					counter++;
					sendcmd(fd,1,id); //SET
					setAlarm();
				}
				//precisamos de checkar se a frame que recebemos é válida, fazer uma estrutura para a frame em si
				if(rcvmachine(fd,id)){
					received=true;
					printf("\tExchanging data...\n");
					break;
				}
			}
			if(counter < linkinfo.numTransmissions && received==true){
				stopAlarm();
				printf("\tConnected with receiver!\n");
			}
			else{
				printf("\tError, reached max retries!\n");
				return -1;
			}
		}
		else if(id == RECEIVER){
			if(rcvmachine(fd , id)){
				//ver porque não posso mandar a que recebi
				sendcmd(fd,2,id); //UA
				printf("\tExchanging data...\n");
				printf("\tConnected with sender!\n");
			}
			else{
				printf("\tCan't connect with sender!\n");
				return -1;
			}
		}
	return fd;
}


int llwrite(int fd, unsigned char* buffer, int length) {
	int counter = 0;

	while(counter < linkinfo.numTransmissions) {
		if (counter == 0 || buzz) {
			buzz = 0;
			sendDataFrame(fd, buffer, length);
			counter++;

			setAlarm();
		}

		//receivedFrame = receiveFrame(al->fd); , preciso de criar a função

		//falta llwrite

	}

	return 0;
}


int sendcmd(int fd, int frametype, int id) {
	unsigned char frame[FRAME_SIZE];

	frame[0] = FLAG;
	frame[4] = FLAG;

	switch(frametype) {
		case 1:
		//SET
			frame[1] = getA(1,id);
			frame[2] = CSET;
			frame[3] = frame[1] ^ frame[2];
			break;
		case 2:
		//UA
			frame[1] = getA(2,id);
			frame[2] = CUA;
			frame[3] = frame[1] ^ frame[2];
			break;
		case 3:
		//DISC
			frame[1] = getA(1,id);
			frame[2] = CDISC;
			frame[3] = frame[1] ^ frame[2];
			break;
		//ver rr e rej
		default:
			printf("\tERROR sending frame (unexpected frame)\n");
			exit(-1);
			break;
	}
	printf("\tFrame setup complete!\n");
	if(frame[1] == 0) {
		printf("\tERROR sending frame (unnexpected status)\n");
		exit(-1);
	}

	if (write(fd, frame, FRAME_SIZE) != FRAME_SIZE) {
		printf("\tERROR sending frame (size don't match)\n");
		exit(-1);
	}
	printf("\tI wrote the S frame...\n");
	return 0;
}

unsigned char getA(int type,int id){
	switch(type){
		case 1:
			if(id == SENDER)
				return A03;
			else if(id==RECEIVER)
				return A01;
			else{
				printf("\tERROR setting up frame\n");
				exit(-1);
			}
			break;
		case 2:
			if(id==SENDER)
				return A01;
			else if( id == RECEIVER )
				return A03;
			else{
				printf("\tERROR setting up frame\n");
				exit(-1);
			}
			break;
	}
	return 0;
}

int rcvmachine(int fd , int id){
	int frameloading=0 , state=0;
	int c0=0,c1=0,c2=0,c3=0,c4=0;
	unsigned char info;
	printf("\tI'm in state machine...\n");
	//state machine done
	while(frameloading!=1){
		info=callRead(fd);
		switch(state){
			case 0:
				if(info==FLAG){
					reader.frame[state]=info;
					state++;
					if(c0==0){
						printf("\t[statemachine] Flag set!\n");
						c0++;
					}
				}
				else{
					if(id == RECEIVER)
						state=0;
					else if(id == SENDER){
						return 0;
					}
				}
				break;
			case 1:
				if(info==A01 || info==A03){
					reader.frame[state]=info;
					state++;
					if(c1==0){
						printf("\t[statemachine] A set!\n");
						c1++;
					}
				}
				else if(info!=FLAG){
					state=0;
				}
				break;
			case 2:
				if(info==CSET || info==CUA || info==CRR || info==CREJ || info==CDISC){
					reader.frame[state]=info;
					state++;
					if(c2==0){
						printf("\t[statemachine] C set!\n");
						c2++;
					}
				}
				else if(info==A01 || info==A03){
					state=2;
				}
				else{
					state=0;
				}
				break;
			case 3:
				if(info==(reader.frame[1]^reader.frame[2])){
					reader.frame[state]=info;
					state++;
					if(c3==0){
						printf("\t[statemachine] BCC set!\n");
						c3++;
					}
				}
				else if(info==A01 || info==A03){
					state=2;
				}
				else if(info==CSET || info==CUA || info==CRR || info==CREJ || info==CDISC){
					state=3;
				}
				else{
					state=0;
				}
				break;
			case 4:
				if(info==FLAG){
					reader.frame[state]=info;
					state++;
					if(c4==0){
						printf("\t[statemachine] Flag set!\n");
						c4++;
					}
					reader.frame[state]='\0';
					frameloading=1;
				}
				else if(info==A01 || info==A03){
					state=2;
				}
				else if(info==CSET || info==CUA || info==CRR || info==CREJ || info==CDISC){
					state=3;
				}
				else if(info==(reader.frame[1]^reader.frame[2])){
					state=4;
				}
				break;
			default:
				return 0;
		}
	}
	return 1;
}

unsigned char callRead(int fd){
	unsigned char info;
	int res=0;
	while(res==0){
		res=read(fd,&info,1);
	}
	printf("\tGot info!\n");
	return info;
}

int openport(char* sport){
	int fd;
    printf("\tOpening serial port..\n");
   	fd = open(sport, O_RDWR | O_NOCTTY);
    if (fd <0) {perror(sport); exit(-1); }
    return fd;
}
//get why
int sendDataFrame(int fd, unsigned char* data, unsigned int size) {
	Frame df;
	df.size =  size + DATA_FRAME_SIZE;
	
	df.frame[0] = FLAG;
	df.frame[1] = A03;
	df.frame[2] = linkinfo.sn << 5;
	df.frame[3] = df.frame[1] ^ df.frame[2];
	memcpy(&df.frame[4], data, size);
	df.frame[4 + size] = getBCC2(data, size);
	df.frame[5 + size] = FLAG;

	//df = stuff(df); ver stuff
	
	if (write(fd, df.frame, df.size) != df.size) {
		printf("\tERROR sending data frame\n");
		return -1;
	}

	return 0;
}

unsigned char getBCC2(unsigned char* data, unsigned int size) {
	unsigned char BCC = 0;

	int i;
	for (i = 0; i < size; i++)
		BCC ^= data[i];

	return BCC;
}