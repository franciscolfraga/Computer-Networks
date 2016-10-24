#ifndef APP_LAYER_H_   /* Include guard */
#define APP_LAYER_H_

typedef struct {
	int fd;
	int status;
	FILE * file;
} ApplicationLayer;

extern ApplicationLayer* al;

typedef enum {
	PARAM_SIZE = 0, PARAM_NAME = 1
} CtrlPckgParam;

int initAppLayer(char * port, int status, char * filePath, int timeout, int retries, int pktSize, int baudrate);

FILE * openFile(char * filePath);

int sendData(char * filePath, int fileSize);
int receiveData(char * filePath);

int sendCtrlPkt(int ctrlField, char * filePath, int fileSize);
int rcvCtrlPkt(int controlField, int * fileSize, char ** filePath);

int sendDataPkt(char * buffer, int bytesRead, int i);
int rcvDataPkt(unsigned char ** buffer,int i);

void printStatistics();

int get_table_nr(int choice);

int get_id();

char* get_serial_port();

int get_retries();

int get_timeout();

int get_packet_size();

int get_baudrate();

char* get_file_name(int status);

void printProgressBar(char * fileName, int bytes, int size, int status);

void printWaiting(int status);

void clrscr();

char* s_file();

char* r_file();

char* searchcd(char* filename);

void printprofile(int id, int packet_size, int baudrate, int nr_retries, int timeout, char* port, char* file);

#endif // APP_LAYER_H_