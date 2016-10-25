#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <signal.h>

#include "link_layer.h"
#include "app_layer.h"
#include "alarm.h"

info_link_layer* link_info;

int get_link_layer(char* port, int baudrate, unsigned int packet_size, unsigned int timeout, unsigned int retries) {
	link_info = (info_link_layer*) malloc(sizeof(info_link_layer));

	link_info->timeout = timeout;
	link_info->stats_info.timeout = 0;

	link_info->pktSize = packet_size;
	link_info->stats_info.msg_sent = 0;
	link_info->stats_info.msg_rcv = 0;

	link_info->baudRate = baudrate;
	link_info->sn = 0;

	link_info->numRetries = retries;
	link_info->stats_info.rr_sent = 0;
	link_info->stats_info.rr_rcv = 0;
	link_info->stats_info.rej_sent = 0;
	link_info->stats_info.rej_rcv = 0;

	strcpy(link_info->port, port);

	if (setNewTermios() < 0)
    	return ERROR;

    return 0;
}

int getBaudrateChoice(int choice) {
	switch (choice) {
	case 0:
		return B0;
	case 75:
		return B75;
	case 150:
		return B150;
	case 300:
		return B300;
	case 600:
		return B600;
	case 1200:
		return B1200;
	case 1800:
		return B1800;
	case 2400:
		return B2400;
	case 4800:
		return B4800;
	case 9600:
		return B9600;
	case 19200:
		return B19200;
	case 38400:
		return B38400;
	case 57600:
		return B57600;
	case 115200:
		return B115200;
	case 230400:
		return B230400;
	case 460800:
		return B460800;
	default:
		return -1;
	}
}

int openSerialPort(char* port) {
	return open(port, O_RDWR | O_NOCTTY);
}

int closeSerialPort() {
	// set old settings
	if (tcsetattr(app_info->fd, TCSANOW, &oldtio) < 0) {
		printf("ERROR in closeSerialPort(): could not set old termios\n");
		return ERROR;
	}

	close(app_info->fd);

	return 0;
}

int setNewTermios() {
	// save current port settings
	if (tcgetattr(app_info->fd, &oldtio) < 0) {
		printf("ERROR in setNewTermios(): could not get old termios\n");
		return ERROR;
	}
    
    // set new termios

    bzero(&newtio, sizeof(newtio));
    newtio.c_cc[VTIME] = 0;   
    newtio.c_cc[VMIN] = 1; 

    newtio.c_cflag = link_info->baudRate | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
	newtio.c_lflag = 0;
  


    if (tcflush(app_info->fd, TCIOFLUSH) < 0) {
    	printf("Error setting up new termios, could not flush non-transmitted output data\n");
    	return ERROR;
    }

    if (tcsetattr(app_info->fd, TCSANOW, &newtio) < -1) {
    	printf("Error setting up new termios, could not set new termios\n");
    	return ERROR;
    }
    return 0;
}

int llopen() {
	int counter = 0;

	switch(app_info->id){
		case SENDER:
			while(counter < link_info->numRetries) {
				if (counter == 0 || buzz) {
					buzz = 0;
					sendCommand(app_info->fd, SET);
					counter++;

					setAlarm();
				}

				if (isCommand(receiveFrame(app_info->fd), UA)) {
					counter--;
					break;
				}
			}

			stopAlarm();
			if (counter < link_info->numRetries)
				printf("Connection successfully established!\n");
			else {
				printf("Could not establish a connection: maximum number of retries reached\n");
				return ERROR;
			}
			break;
		case RECEIVER:
			if (isCommand(receiveFrame(app_info->fd), SET)) {
				sendCommand(app_info->fd, UA);
				printf("Connection successfully established!\n");
			}
			break;
		default:
			break;
	}

	return 0;
}

int llclose() {
	int counter = 0;

	switch(app_info->id) {
		case SENDER:
			while(counter < link_info->numRetries) {
				if (counter == 0 || buzz) {
					buzz = 0;
					sendCommand(app_info->fd, DISC);
					counter++;
					setAlarm();
					if (isCommand(receiveFrame(app_info->fd), DISC)) {
						sendCommand(app_info->fd, UA);
						sleep(1);
						break;
					}
				}
			}

			stopAlarm();
			if (counter < link_info->numRetries)
				printf("Connection successfully terminated!\n");
			else {
				printf("Could not disconect: maximum number of retries reached\n");
				return ERROR;
			}
			break;
		case RECEIVER:
			while (!isCommand(receiveFrame(app_info->fd), DISC))
				continue;

			while(counter < link_info->numRetries) {
				if (counter == 0 || buzz) {
					buzz = 0;
					counter++;
					setAlarm();
					sendCommand(app_info->fd, DISC);
					if (isCommand(receiveFrame(app_info->fd), UA))
						break;
				}
			}
			stopAlarm();
			if (counter < link_info->numRetries)
				printf("Connection successfully terminated!\n");
			else {
				printf("Could not disconect: maximum number of retries reached\n");
				return ERROR;
			}
			break;
		default:
			break;
	}

	return 0;
}


int llwrite(unsigned char* buf, int bufSize) {
	int counter = 0;
	Frame receivedFrame;

	while(counter < link_info->numRetries) {
		if (counter == 0 || buzz) {
			buzz = 0;
			sendDataFrame(app_info->fd, buf, bufSize);
			counter++;

			setAlarm();
		}

		receivedFrame = receiveFrame(app_info->fd);

		if (isCommand(receivedFrame, RR)) {
			link_info->stats_info.rr_sent++;

			if(link_info->sn != receivedFrame.sn)
				link_info->sn = receivedFrame.sn;

			stopAlarm();
			counter--;
			break;

		} else if (isCommand(receivedFrame, REJ)) {
			link_info->stats_info.rej_sent++;
			counter = 0;
			stopAlarm();
		}

	}

	if (counter >= link_info->numRetries) {
		printf("Could not send frame: maximum number of retries reached\n");
		stopAlarm();
		return ERROR;
	}

	return 0;
}

int llread(unsigned char ** message) {
	int disc = 0, dataSize;
	Frame frm;

	while (!disc) {
		frm = receiveFrame(app_info->fd);
		
		switch (frm.type) {
			case COMMAND:
				if (isCommand(frm, DISC))
					disc = 1;

				break;
			case DATA:
				if (frm.answer == RR && link_info->sn == frm.sn) {
					link_info->stats_info.rr_rcv++;
					link_info->sn = !frm.sn;
					dataSize = frm.size - DATA_FRAME_SIZE;
					*message = malloc(dataSize);
					memcpy(*message, &frm.frame[4], dataSize);
					disc = 1;
				}
				else if (frm.answer == REJ) {
					link_info->stats_info.rej_rcv++;
					link_info->sn = frm.sn;
				}

				if (frm.answer != NONE)
					sendCommand(app_info->fd, frm.answer);
				break;
			case INVALID:
				break;
			default:
				return ERROR;
		}

	}

	return 0;
}

int isCommand(Frame frm, Command cmd) {
	if (frm.type == INVALID)
		return 0;

	switch (frm.frame[2] & 0x0F) {
	case C_SET:
		if (cmd == SET)
			return 1;
		else
			return 0;
	case C_UA:
		if (cmd == UA)
			return 1;
		else
			return 0;
	case C_RR:
		if (cmd == RR)
			return 1;
		else
			return 0;
	case C_REJ:
		if (cmd == REJ)
			return 1;
		else
			return 0;
	case C_DISC:
		if (cmd == DISC)
			return 1;
		else
			return 0;
	default:
		return 0;
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

int sendDataFrame(int fd, unsigned char* data, unsigned int size) {
	Frame df;
	df.size =  size + DATA_FRAME_SIZE;
	
	df.frame[0] = FLAG;
	df.frame[1] = A03;
	df.frame[2] = link_info->sn << 5;
	df.frame[3] = df.frame[1] ^ df.frame[2];
	memcpy(&df.frame[4], data, size);
	df.frame[4 + size] = getBCC2(data, size);
	df.frame[5 + size] = FLAG;

	df = stuff(df);
	
	if (write(fd, df.frame, df.size) != df.size) {
		printf("ERROR in sendDataFrame(): could not send frame\n");
		return ERROR;
	}

	return 0;
}

int sendCommand(int fd, Command cmd) {
	unsigned char frame[FRAME_SIZE];

	frame[0] = FLAG;
	frame[4] = FLAG;

	switch(cmd) {
		case SET:
			frame[1] = getAFromCmd();
			frame[2] = C_SET;
			frame[3] = frame[1] ^ frame[2];
			break;
		case UA:
			frame[1] = getAFromRspn();
			frame[2] = C_UA;
			frame[3] = frame[1] ^ frame[2];
			break;
		case DISC:
			frame[1] = getAFromCmd();
			frame[2] = C_DISC;
			frame[3] = frame[1] ^ frame[2];
			break;
		case RR:
			frame[1] = getAFromRspn();
			frame[2] = C_RR;
			frame[2] |= (link_info->sn << 5);
			frame[3] = frame[1] ^ frame[2];
			break;
		case REJ:
			frame[1] = getAFromRspn();
			frame[2] = C_REJ;
			frame[2] |= (link_info->sn << 5);
			frame[3] = frame[1] ^ frame[2];
			break;
		default:
			printf("ERROR in sendFrame(): unexpected frame\n");
			break;
	}

	if(frame[1] == 0) {
		printf("ERROR in sendFrame(): unnexpected status\n");
		return ERROR;
	}

	if (write(fd, frame, FRAME_SIZE) != FRAME_SIZE) {
		printf("ERROR in sendFrame(): could not send frame\n");
		return ERROR;
	}

	return 0;
}

unsigned char getAFromCmd() {
	switch(app_info->id){
		case SENDER:
			return A03;
		case RECEIVER:
			return A01;
	}
	return 0;
}

unsigned char getAFromRspn() {
	switch(app_info->id){
		case SENDER:
			return A01;
		case RECEIVER:
			return A03;
	}
	return 0;
}

Frame receiveFrame(int fd) {
	unsigned char c;
	int res, receiving = 1, state = 0, dataFrame = 0, i = 0;
	Frame frm;
	frm.type = INVALID;

	while(receiving) {
		res = read(fd, &c, 1);
		
		if (res < 1)
			return frm;

		switch(state) {
		case 0:
			if (c == FLAG) {
				frm.frame[i] = c;
				i++;
				state++;
			}
			break;
		case 1:
			if (c == A01 || c == A03) {
				frm.frame[i] = c;
				i++;
				state++;
			}
			else if (c != FLAG) {
				state = 0;
				i = 0;
			}
			break;
		case 2: 
			if (c != FLAG) {
				frm.frame[i] = c;
				i++;
				state++;
			}
			else if (c == FLAG) {
				state = 1;
				i = 1;
			}
			else {
				state = 0;
				i = 0;
			}
			break;
		case 3:
			if (c == (frm.frame[1]^frm.frame[2])) {
				frm.frame[i] = c;
				i++;
				state++;
			}
			else if (c == FLAG) {
				state = 1;
				i = 1;
			}
			else {
				state = 0;
				i = 0;
			}
			break;
		case 4:
			if (c == FLAG) {
				frm.frame[i] = c;
				i++;
				frm.frame[i] = 0;
				receiving = 0;

				if (i > 5)
					dataFrame = 1;
			}
			else {
				frm.frame[i] = c;
				i++;
			}
			break;
		default:
			break;
		}
	}

	if (dataFrame) {
		frm.size = i;
		frm.answer = NONE;
		frm.type = DATA;
		frm = destuff(frm);
		
		// check BCC1
		if (frm.frame[3] != (frm.frame[1] ^ frm.frame[2])) {
			printf("ERROR in receiveFrame(): BCC1 error\n");
			return frm;
		}
		
		// check BCC2
		int dataSize = frm.size - DATA_FRAME_SIZE;
		unsigned char BCC2 = getBCC2(&frm.frame[4], dataSize);
		if (frm.frame[4 + dataSize] != BCC2) {			
			printf("ERROR in receiveFrame(): BCC2 error\n");
			frm.answer = REJ;
		}

		if (frm.answer == NONE)
			frm.answer = RR;

		frm.sn = (frm.frame[2] >> 5) & BIT(0);
	}
	else {
		frm.type = COMMAND;
		if (isCommand(frm, RR) || isCommand(frm, REJ))
			frm.sn = (frm.frame[2] >> 5) & BIT(0);
	}
	return frm;
}

Frame stuff(Frame df) {
	Frame stuffed_frame;
	unsigned int newSize = df.size;

	int i;
	for (i = 1; i < (df.size - 1); i++) {
		if (df.frame[i] == FLAG || df.frame[i] == ESCAPE)
			newSize++;
	}

	stuffed_frame.frame[0] = df.frame[0];
	int j = 1;
	for (i = 1; i < (df.size - 1); i++) {
		if (df.frame[i] == FLAG || df.frame[i] == ESCAPE) {
			stuffed_frame.frame[j] = ESCAPE;
			stuffed_frame.frame[++j] = df.frame[i] ^ 0x20;
		}
		else
			stuffed_frame.frame[j] = df.frame[i];
		j++;
	}
	
	stuffed_frame.size = newSize;
	stuffed_frame.frame[j] = df.frame[i];
	
	return stuffed_frame;
}

Frame destuff(Frame df) {
	Frame destuffed_frame;
	destuffed_frame.answer = df.answer;
	destuffed_frame.sn = df.sn;
	destuffed_frame.type = df.type;

	int size = 0;
	
	int i;

	for (i = 0; i < df.size; i++) {
		if (df.frame[i] == ESCAPE)
			destuffed_frame.frame[size] = df.frame[++i] ^ 0x20;
		else
			destuffed_frame.frame[size] = df.frame[i];
		size++;
	}

	destuffed_frame.size = size;

	return destuffed_frame;
}
