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
#include <dirent.h>

#include "link_layer.h"
#include "app_layer.h"

info_app_layer* app_info;

int real=-1;


int get_id() {
	int id = ERROR;
	while (id != 1 && id != 2) {
		printf("\tPlease help me to identify you:\n\n");
		printf("\t1) I'm the file sender\n");
		printf("\t2) I'm the file receiver\n\t");
		scanf("%d",&id);
	}
	return id-1;		
}


char* get_serial_port() {
	int port = ERROR;
	while (port != 1 && port != 2) {
		printf("\tSerial ports available:\n");
		printf("\t1) /dev/ttyS0\n");
		printf("\t2) /dev/ttyS1\n");
		printf("\tChoose the serial port to exchange data: ");
		scanf("%d",&port);
	}
	if (port==1)
		return "/dev/ttyS0";
	else
		return "/dev/ttyS1";
}

int get_retries() {
	int retries=0;
	while (retries < 1) {
		printf("\tHow many retries can I get to send a packet?\n\n\t");
		scanf("%d", &retries);
	}
	return retries;
}

int get_timeout() {
	int timeout=0;
	while (timeout < 1) {
		printf("\tHow many seconds do I have between retries?\n\n\t");
		scanf("%d", &timeout);
	}
	return timeout;
}

int get_packet_size() {
	int packet_size = ERROR;
	while (packet_size < 1 || packet_size > 512) {
		printf("\tWhat is the maximum packet size I can exchange?\n\n\t");
		scanf("%d", &packet_size);
	}
	return packet_size;
}

int get_baudrate() {
  	int choice = ERROR;
  	while (real < 0) {
		printf("\tPlease choose a baudrate value...\n\n");
		printf("\t1) 0\t\t2) 75\t\t3) 150\t\t4) 300\n\n\t5) 600\t\t6) 1200\t\t7) 1800\t\t8) 2400\n\n\t9) 4800\t\t10) 9600\t11) 19200\t12) 38400\n\n\t13) 57600\t14) 115200\t15) 230400\t16) 460800\n\n\t");
	
		scanf("%d", &choice);
		real=get_table_nr(choice);
		printf("Realchoice:%d\n",real);
  	}
  return getBaudrateChoice(real);
}

int get_table_nr(int choice){

int realchoice=10;	//default

switch(choice){
  		case 1:
  			realchoice=0;
  			break;
  		case 2:
  			realchoice=75;
  			break;
  		case 3:
  			realchoice=150;
  			break;
  		case 4:
  			realchoice=300;
  			break;
  		case 5:
  			realchoice=600;
  			break;
  		case 6:
  			realchoice=1200;
  			break;
  		case 7:
  			realchoice=1800;
  			break;
  		case 8:
  			realchoice=2400;
  			break;
  		case 9:
  			realchoice=4800;
  			break;
  		case 10:
  			realchoice=9600;
  			break;
  		case 11:
  			realchoice=19200;
  			break;
  		case 12:
  			realchoice=38400;
  			break;
  		case 13:
  			realchoice=57600;
  			break;
  		case 14:
  			realchoice=115200;
  			break;
  		case 15:
  			realchoice=230400;
  			break;
  		case 16:
  			realchoice=460800;
  			break;
  		default:
  			realchoice=-1;
  }
  return realchoice;

}


char* get_file_name(int id) {
	char* file_name="default";
	if(id==SENDER)
		file_name=s_file();
	else
		file_name=r_file();

	return file_name;
}


void print_progress(char * fileName, int bytes, int size, int id) {
	clrscr();
	if (id == 0)
		printf("Sending %s...\n\n", fileName);
	else if (id == 1)
		printf("Receiving %s...\n\n", fileName);
	
	printf("\t%d / %d bytes\n\t%d %%\n\n", bytes, size, (int)((float)bytes / (float)size * 100));
}

void print_wait(int id) {
	printf("\n\n\n\n");
	if (id == 0)
		printf("\tWaiting for receiver...\n\n");
	else
		printf("\tWaiting for sender...\n\n");	
}

void clrscr() {
	printf("\033[2J");
} 


char* s_file(){
	//all files in directory
	DIR *d;
	printf("\tHere I display all the files in this directory:\n");
	struct dirent *dir;
	char** filename=malloc(sizeof(*filename)*100);
	d = opendir(".");
	int cont=0;
	if (d){
		while ((dir = readdir(d)) != NULL){
			printf("\t\t%d) %s\n",cont+1, dir->d_name);
			filename[cont]=dir->d_name;
			cont++;
		}
    printf("\t");
    closedir(d);
	}
	int filenr;
		printf("Choose one to send: ");
	scanf("%d",&filenr);
	while(filenr<1 || filenr>cont){
		printf("\tError!! Please choose a number between 1 and %d: ", cont);
		scanf("%d",&filenr);
	}
	printf("\n");
	char *finalfile=filename[filenr-1];
	return finalfile;
}

char* r_file(){
	printf("\tPlease type the output name of the file you will receive.\n ");
	char* file=malloc(sizeof(file)*100);
	scanf("%s",file);
	if(strcmp(searchcd(file),"ok")!=0){
            printf("\tFile with name (%s) found in current directory!\n", file);
            printf("\tDo you wish to continue? (1 for Yes, 2 for NO!) ");
            int answer=0;
            scanf("%d",&answer);
            while(answer!=1 && answer!=2){
                printf("\n\tError, you can only answer 1 or 2 ");
                scanf("%d",&answer);
            }
            switch(answer){
                case 1: 
                    printf("\n");
                    break;
                case 2:
                    printf("\n\tOK, try again!\n");
                    return get_file_name(RECEIVER);
            }
        }
	return file;
}

char* searchcd(char* filename){
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d){
        while ((dir = readdir(d)) != NULL){
            if(strcmp(filename,dir->d_name)==0){
                return dir->d_name;
            }
        }
    closedir(d);
    }
    return "ok";
}

void printprofile(int id, int packet_size, int baudrate, int nr_retries, int timeout, char* port, char* file){
	switch(id){
		case 0:
			printf("\tID: Sender\n");
			if(file!=NULL){
				printf("\tFile to send: %s\n", file);
			}
			break;
		case 1: 
			printf("\tID: Receiver\n");
			if(file!=NULL){
				printf("\tFile to receive: %s\n", file);
			}
			break;

	}
	if(port!=NULL)
		printf("\tSerial port chosen: %s\n", port);
	if(packet_size!=-1)
		printf("\tPacket size chosen: %d\n", packet_size);
	if(baudrate!=-1)
		printf("\tBaudrate chosen: %d\n", real);
	if(nr_retries!=-1)
		printf("\tNumber of retries allowed: %d\n", nr_retries);
	if(timeout!=-1){
		printf("\tTimeout between try: %d\n", timeout);
	}
	printf("\n\n\n\n");
}


int get_app(char* port, int id, char* file,int timeout, int retries, int packet_size, int baudrate) {
	app_info = (info_app_layer*) malloc(sizeof(info_app_layer));
	app_info->fd = openSerialPort(port);
	
	if (app_info->fd < 0) {
		printf("Error, could not open serial port\n");
		return ERROR;
	}
	app_info->id = id;

	app_info->file = open_file(file);
	
	int file_size;

	if (app_info->id == SENDER) {
		struct stat st; 
		if (stat(file, &st) == 0)
			file_size = st.st_size;
		else {
			printf("Error, could not get file size!\n");
			return ERROR;
		}
	}
	
	if (app_info->file == NULL )
		return ERROR;	

	if (get_link_layer(port, baudrate, packet_size, timeout, retries) < 0) {
		printf("Error in application layer, could not initialize link layer\n");
		return ERROR;
	}

	print_wait(app_info->id);
	
	if (llopen() == ERROR)
		return ERROR;

	if (app_info->id == SENDER)
		sendData(file, file_size);
	else if (app_info->id == RECEIVER)
		receiveData(file);
	
	llclose();
	closeSerialPort();
	
	print_stats();

	return 0;
		
}

FILE * open_file(char * file_path) {
	
	FILE * file;
	
	if(app_info->id == SENDER) 
		file = fopen(file_path, "rb");
	else 
		file = fopen(file_path, "wb");
		
	if(file == NULL) {
		printf("Error opening file (%s)\n", file_path);
		return NULL;
	}

	return file;	
}

int sendData(char* file, int fileSize) {

	if (sendCtrlPkt(CTRL_PKT_START, file, fileSize) < 0)
		return ERROR;
	
	link_info->stats_info.msg_sent++;

	int bytesRead = 0, i = 0, bytesAcumulator = 0;;
	char * buffer = malloc(link_info->pktSize * sizeof(char));

	while((bytesRead = fread(buffer, sizeof(char), link_info->pktSize, app_info->file)) > 0){
		if(sendDataPkt(buffer, bytesRead, i) < 0)
			return ERROR;

		link_info->stats_info.msg_sent++;
		i++;
		if (i > 207)
			i = 0;
		bytesAcumulator += bytesRead;
		print_progress(file, bytesAcumulator, fileSize, 0);
	}

	if (fclose(app_info->file) < 0) {
		printf("Error sending data, error closing file!\n");
		return ERROR;
	}

	if (sendCtrlPkt(CTRL_PKT_END, file, fileSize) < 0)
		return ERROR;

	link_info->stats_info.msg_sent++;

	printf("File sent!\n");

	return 0;
}



int receiveData(char * filePath) {
	int fileSize;

	if(rcvCtrlPkt(CTRL_PKT_START, &fileSize, &filePath) < 0)
		return ERROR;

	link_info->stats_info.msg_rcv++;

	int bytesRead, bytesAcumulator = 0, i = 0;
	unsigned char * buffer = malloc(link_info->pktSize * sizeof(char));

	while (bytesAcumulator < fileSize){
		bytesRead = rcvDataPkt(&buffer, i);
		printf("%d\n", bytesRead);
		if(bytesRead < 0)
			return ERROR;
		link_info->stats_info.msg_rcv++;
		bytesAcumulator += bytesRead;
		fwrite(buffer, sizeof(char), bytesRead, app_info->file);
		i++;
		if (i > 207)
			i = 0;
		print_progress(filePath, bytesAcumulator, fileSize, 1);
	}

	if (fclose(app_info->file) < 0) {
		printf("Error sending data error closing file!\n");
		return ERROR;
	}

	if (rcvCtrlPkt(CTRL_PKT_END, &fileSize, &filePath) < 0)
		return ERROR;

	link_info->stats_info.msg_rcv++;

	printf("File received!\n");

	return 0;
}


int sendCtrlPkt(int ctrlField, char * filePath, int fileSize) {

	char sizeString[16];
	sprintf(sizeString, "%d", fileSize);

	int size = 5 + strlen(sizeString) + strlen(filePath);

	unsigned char ctrlPckg[size];

	ctrlPckg[0] = ctrlField + '0';
	ctrlPckg[1] = PARAM_SIZE + '0';
	ctrlPckg[2] = strlen(sizeString) + '0';


	int i, acumulator = 3;
	for(i = 0; i < strlen(sizeString); i++) {
		ctrlPckg[acumulator] = sizeString[i];
		acumulator++;;
	}

	ctrlPckg[acumulator] = PARAM_NAME + '0';
	acumulator++;
	ctrlPckg[acumulator] = strlen(filePath) + '0';
	acumulator++;

	for(i = 0; i < strlen(filePath); i++) {
		ctrlPckg[acumulator] = filePath[i];
		acumulator++;;
	}

	if (llwrite(ctrlPckg, size) < 0) {
		printf("Error sending control packet, llwrite() function error!\n");
		return ERROR;
	}

	return 0;
}

int sendDataPkt(char * buffer, int bytesRead, int i) {

	int size = bytesRead + 4;
	unsigned char dataPckg[size];

	dataPckg[0] = CTRL_PKT_DATA + '0';
	dataPckg[1] = i + '0';

	dataPckg[2] = bytesRead / 256;
	dataPckg[3] = bytesRead % 256;
	memcpy(&dataPckg[4], buffer, bytesRead);

	if (llwrite(dataPckg, size) < 0) {
		printf("Error sending data packet, llwrite() function error!\n");
		return ERROR;
	}

	return 0;
}


int rcvCtrlPkt(int controlField, int * fileSize, char ** filePath) {

	unsigned char * info;

	if (llread(&info) < 0) {
		printf("Error receiveing control packet\n");
		return ERROR;
	}
	
	if ((info[0] - '0') != controlField) {
		printf("Error receiveing control packet, unexpected control field!\n");
		return ERROR;
	}

	if ((info[1] - '0') != PARAM_SIZE) {
		printf("Error receiveing control packet, unexpected size parameter!\n");
		return ERROR;
	}

	int i, fileSizeLength = (info[2] - '0'), acumulator = 3;

	char fileSizeStr[MAX_STR_SIZE];

	for(i = 0; i < fileSizeLength; i++) {
		fileSizeStr[i] = info[acumulator];
		acumulator++;
	}

	fileSizeStr[acumulator - 3] = '\0';

	(*fileSize) = atoi(fileSizeStr);

	if((info[acumulator] - '0') != PARAM_NAME) {
		printf("Error receiveing control packet, unexpected name param!\n");
		return ERROR;
	}

	acumulator++;
	
	int pathLength = (info[acumulator] - '0');
	acumulator++;

	char pathStr[MAX_STR_SIZE];
	
	for(i = 0; i < pathLength; i++) {
		pathStr[i] = info[acumulator];
		acumulator++;
	}

	pathStr[i] = '\0';
	strcpy((*filePath), pathStr);

	return 0;
}

int rcvDataPkt(unsigned char ** buffer,int i) {

	unsigned char * info = NULL;
	int bytes = 0;

	if (llread(&info) < 0) {
		printf("Error receiveing data packet, llread() function error!\n");
		return ERROR;
	}

	if (info == NULL)
		return 0;

	int C = info[0] - '0';
	int N = info[1] - '0';

	if (C != CTRL_PKT_DATA) {
		printf("Error receiveing data packet, control field it's different from CTRL_PKT_DATA!\n");
		return ERROR;
	}
	
	if (N != i) {
		printf("Error receiveing data packet, sequence number it's wrong!\n");
		return ERROR;
	}

	int L2 = info[2], L1 = info[3];
	bytes = 256 * L2 + L1;

	memcpy((*buffer), &info[4], bytes);

	free(info);

	return bytes;
}

void print_stats() {
	printf("\n");
	printf("\t-- Ocurrences Registration --\n\n\n");
	printf("\tTimeouts: %d\n", link_info->stats_info.timeout);
	printf("\tMessages sent: %d\n", link_info->stats_info.msg_sent);
	printf("\tRR frames sent: %d\n", link_info->stats_info.rr_sent);
	printf("\tREJ frames sent: %d\n", link_info->stats_info.rej_sent);
	printf("\tMessages received: %d\n", link_info->stats_info.msg_rcv);
	printf("\tRR frames received: %d\n", link_info->stats_info.rr_rcv);
	printf("\tREJ frames received: %d\n\n\n\n", link_info->stats_info.rej_rcv);
}
