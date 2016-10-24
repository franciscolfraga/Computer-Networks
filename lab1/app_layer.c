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

ApplicationLayer* al;

int real=-1;

int initAppLayer(char* port, int status, char * filePath,int timeout, int retries, int pktSize, int baudrate) {
	al = (ApplicationLayer*) malloc(sizeof(ApplicationLayer));

	al->fd = openSerialPort(port);
	
	if (al->fd < 0) {
		printf("ERROR in initAppLayer(): could not open serial port\n");
		return ERROR;
	}
	al->status = status;

	al->file = openFile(filePath);
	
	int fileSize;

	if (al->status == SENDER) {
		struct stat st; 
		if (stat(filePath, &st) == 0)
			fileSize = st.st_size;
		else {
			printf("ERROR getting file size!\n");
			return ERROR;
		}
	}
	
	if (al->file == NULL )
		return ERROR;	

	if (initLinkLayer(port, baudrate, pktSize, timeout, retries) < 0) {
		printf("ERROR in initAppLayer(): could not initialize link layer\n");
		return ERROR;
	}
	
	printWaiting(al->status);
	
	if (llopen() == ERROR)
		return ERROR;

	if (al->status == SENDER)
		sendData(filePath, fileSize);
	else if (al->status == RECEIVER)
		receiveData(filePath);
	
	llclose();
	closeSerialPort();
	
	//printStatistics();

	return 0;
		
}

FILE * openFile(char * filePath) {
	
	FILE * file;
	
	if(al->status == SENDER) file = fopen(filePath, "rb");
	else file = fopen(filePath, "wb");
		
	if(file == NULL) {
		printf("ERROR in openFile(): error opening file with path <%s>\n", filePath);
		return NULL;
	}

	return file;	
}

int sendData(char * filePath, int fileSize) {

	if (sendCtrlPkt(CTRL_PKT_START, filePath, fileSize) < 0)
		return ERROR;
	
	ll->statistics.msgSent++;

	int bytesRead = 0, i = 0, bytesAcumulator = 0;;
	char * buffer = malloc(ll->pktSize * sizeof(char));

	while((bytesRead = fread(buffer, sizeof(char), ll->pktSize, al->file)) > 0){
		if(sendDataPkt(buffer, bytesRead, i) < 0)
			return ERROR;

		ll->statistics.msgSent++;
		i++;
		if (i > 207)
			i = 0;
		bytesAcumulator += bytesRead;
		printProgressBar(filePath, bytesAcumulator, fileSize, 0);
	}

	if (fclose(al->file) < 0) {
		printf("ERROR in sendData(): error closing file!\n");
		return ERROR;
	}

	if (sendCtrlPkt(CTRL_PKT_END, filePath, fileSize) < 0)
		return ERROR;

	ll->statistics.msgSent++;

	printf("File sent!\n");

	return 0;
}



int receiveData(char * filePath) {
	int fileSize;

	if(rcvCtrlPkt(CTRL_PKT_START, &fileSize, &filePath) < 0)
		return ERROR;

	ll->statistics.msgRcvd++;

	int bytesRead, bytesAcumulator = 0, i = 0;
	unsigned char * buffer = malloc(ll->pktSize * sizeof(char));

	while (bytesAcumulator < fileSize){
		bytesRead = rcvDataPkt(&buffer, i);
		printf("%d\n", bytesRead);
		if(bytesRead < 0)
			return ERROR;
		ll->statistics.msgRcvd++;
		bytesAcumulator += bytesRead;
		fwrite(buffer, sizeof(char), bytesRead, al->file);
		i++;
		if (i > 207)
			i = 0;
		printProgressBar(filePath, bytesAcumulator, fileSize, 1);
	}

	if (fclose(al->file) < 0) {
		printf("ERROR in receiveData(): error closing file!\n");
		return ERROR;
	}

	if (rcvCtrlPkt(CTRL_PKT_END, &fileSize, &filePath) < 0)
		return ERROR;

	ll->statistics.msgRcvd++;

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
		printf("ERROR in sendCtrlPkt(): llwrite() function error!\n");
		return ERROR;
	}

	return 0;
}


int rcvCtrlPkt(int controlField, int * fileSize, char ** filePath) {

	unsigned char * info;

	if (llread(&info) < 0) {
		printf("ERROR in rcvCtrlPkt(): \n");
		return ERROR;
	}
	
	if ((info[0] - '0') != controlField) {
		printf("ERROR in rcvCtrlPkt(): unexpected control field!\n");
		return ERROR;
	}

	if ((info[1] - '0') != PARAM_SIZE) {
		printf("ERROR in rcvCtrlPkt(): unexpected size param!\n");
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
		printf("ERROR in rcvCtrlPkt(): unexpected name param!\n");
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


int sendDataPkt(char * buffer, int bytesRead, int i) {

	int size = bytesRead + 4;
	unsigned char dataPckg[size];

	dataPckg[0] = CTRL_PKT_DATA + '0';
	dataPckg[1] = i + '0';

	dataPckg[2] = bytesRead / 256;
	dataPckg[3] = bytesRead % 256;
	memcpy(&dataPckg[4], buffer, bytesRead);

	if (llwrite(dataPckg, size) < 0) {
		printf("ERROR in sendDataPkt(): llwrite() function error!\n");
		return ERROR;
	}

	return 0;
}

int rcvDataPkt(unsigned char ** buffer,int i) {

	unsigned char * info = NULL;
	int bytes = 0;

	if (llread(&info) < 0) {
		printf("ERROR in rcvDataPkt(): llread() function error!\n");
		return ERROR;
	}

	if (info == NULL)
		return 0;

	int C = info[0] - '0';
	int N = info[1] - '0';

	if (C != CTRL_PKT_DATA) {
		printf("ERROR in rcvDataPkt(): control field it's different from CTRL_PKT_DATA!\n");
		return ERROR;
	}
	
	if (N != i) {
		printf("ERROR in rcvDataPkt(): sequence number it's wrong!\n");
		return ERROR;
	}

	int L2 = info[2], L1 = info[3];
	bytes = 256 * L2 + L1;

	memcpy((*buffer), &info[4], bytes);

	free(info);

	return bytes;
}

void printStatistics() {
	printf("\n");
	printf("### Statistics ###\n\n");
	printf("Timeouts: %d\n\n", ll->statistics.timeout);
	printf("Sent messages: %d\n", ll->statistics.msgSent);
	printf("Received messages: %d\n\n", ll->statistics.msgRcvd);
	printf("Sent RR: %d\n", ll->statistics.rrSent);
	printf("Received RR: %d\n\n", ll->statistics.rrRcvd);
	printf("Sent REJ: %d\n", ll->statistics.rejSent);
	printf("Received REJ: %d\n\n", ll->statistics.rejRcvd);
}


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


void printProgressBar(char * fileName, int bytes, int size, int mode) {
	clrscr();
	if (mode == 0)
		printf("Sending %s...\n\n", fileName);
	else if (mode == 1)
		printf("Receiving %s...\n\n", fileName);
	
	printf("[");
	int i, barSize = 30;
	for (i = 0; i < barSize; i++) {
		if(((float)bytes / (float)size ) > ( (float)i / barSize))
			printf("=");
		else
			printf(" ");
	}
	printf("]");
	printf("  %d %%\t%d / %d bytes\n\n", (int)((float)bytes / (float)size * 100), bytes, size);
}

void printWaiting(int mode) {
	clrscr();
	if (mode == 0)
		printf("Waiting for receiver...\n\n");
	else
		printf("Waiting for sender...\n\n");	
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
			if(file!=NULL)
				printf("\tFile to send: %s\n", file);
			break;
		case 1: 
			printf("\tID: Receiver\n");
			if(file!=NULL)
				printf("\tFile to receive: %s\n", file);
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
	if(timeout!=-1)
		printf("\tTimeout between try: %d\n", timeout);
	printf("\n\n\n\n");
}
