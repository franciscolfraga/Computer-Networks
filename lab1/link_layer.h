#ifndef LINK_LAYER_H_   /* Include guard */
#define LINK_LAYER_H_

#include <termios.h>

#define C_SET 0x03
#define C_UA 0x07
#define C_RR 0x05
#define C_REJ 0x01
#define C_DISC 0x0B

#define FLAG 0x7E
#define A03 0x03
#define A01 0x01
#define ESCAPE 0x7D

#define SENDER 0
#define RECEIVER 1
#define BAUDRATE B38400
#define ERROR -1

#define FRAME_SIZE 5
#define DATA_FRAME_SIZE 6
#define MAX_BUF_SIZE 256
#define MAX_FRAME_SIZE 1024

#define MAX_STR_SIZE 100

#define BIT(n) (0x01 << n)

typedef enum {
	SET, UA, RR, REJ, DISC, NONE
} Command;

typedef enum {
	INVALID, DATA, COMMAND
} FrameType;

typedef struct {
	int timeout;

	int msg_sent;
	int rej_sent;
	int rr_sent;

	int msg_rcv;
	int rr_rcv;
	int rej_rcv;

} Statistics;

typedef struct {
	char port[20];
	int baudRate;
	unsigned int sn;
	unsigned int timeout;
	unsigned int numRetries;
	unsigned int pktSize;
	Statistics stats_info;
} info_link_layer;

typedef struct {
	unsigned char frame[MAX_FRAME_SIZE];
	unsigned int size;
	unsigned int sn;
	FrameType type;
	Command answer;
} Frame;

typedef enum {
	CTRL_PKT_DATA = 1, CTRL_PKT_START = 2, CTRL_PKT_END = 3
} ControlPacketType;

struct termios oldtio, newtio;

extern info_link_layer* link_info;

int get_link_layer(char* port, int baudrate, unsigned int packet_size, unsigned int timeout, unsigned int retries);

int getBaudrateChoice(int choice);

int openSerialPort(char* port);

int closeSerialPort();

int setNewTermios();

int llopen();
int llwrite(unsigned char * buf, int bufSize);
int llread(unsigned char ** message);
int llclose();

unsigned char getBCC2(unsigned char* data, unsigned int size);

int sendDataFrame(int fd, unsigned char* data, unsigned int size);

int sendCommand(int fd, Command cmd);

int isCommand(Frame frm, Command cmd);

unsigned char getAFromCmd();

unsigned char getAFromRspn();

Frame receiveFrame(int fd);

Frame stuff(Frame df);

Frame destuff(Frame df);


#endif // LINK_LAYER_H_