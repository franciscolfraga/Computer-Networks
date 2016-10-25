#ifndef APP_LAYER_H_   /* Include guard */
#define APP_LAYER_H_

typedef enum {
	PARAM_SIZE = 0, PARAM_NAME = 1
} CtrlPckgParam;

typedef struct {
	int fd;
	int id;
	FILE * file;
} info_app_layer;


extern info_app_layer* app_info;

void printprofile(int id, int packet_size, int baudrate, int nr_retries, int timeout, char* port, char* file);

int get_app(char * port, int id, char * filePath, int timeout, int retries, int pktSize, int baudrate);

FILE * open_file(char * filePath);

int sendData(char * filePath, int fileSize);

int receiveData(char * filePath);

int sendCtrlPkt(int ctrlField, char * filePath, int fileSize);

int rcvCtrlPkt(int controlField, int * fileSize, char ** filePath);

int sendDataPkt(char * buffer, int bytesRead, int i);

int rcvDataPkt(unsigned char ** buffer,int i);

void print_stats();

int get_table_nr(int choice);

int get_id();

char* get_serial_port();

int get_retries();

int get_timeout();

int get_packet_size();

int get_baudrate();

char* get_file_name(int id);

void print_progress(char * fileName, int bytes, int size, int id);

void print_wait(int id);

void clrscr();

char* s_file();

char* r_file();

char* searchcd(char* filename);


#endif // APP_LAYER_H_