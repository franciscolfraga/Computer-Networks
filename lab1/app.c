#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include "app.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <strings.h>

void startapp(char* file_name){
    if(appinfo.status == SENDER)
        send(file_name);
    else if(appinfo.status == RECEIVER)
        rcv();
}

void send(char* file_name){
    struct stat st;
    stat(file_name, &st);
    appinfo.file_size = st.st_size;
}
void rcv(){
    
}

int ID(){
	printf("\tPlease help me to identify you:\n\n");
	int id=0;
	while(id!=2 && id!=1){
	printf("\t1 for I'm the file sender\n");
	printf("\t2 for I'm the file receiver\n\t");
	scanf("%d", &id);
}
	return id;
}
char* sFile(){
	//all files in directory
	DIR *d;
	printf("\tHere I display all the files in this directory:\n");
	struct dirent *dir;
	char** filename=malloc(sizeof(*filename)*100);
	d = opendir(".");
	int cont=0;
	if (d){
		while ((dir = readdir(d)) != NULL){
			printf("\t\t%d for %s\n",cont+1, dir->d_name);
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
	free(filename);
	return finalfile;
}
char *rFile(){
	printf("\tPlease type the output name of the file you will receive.\n ");
	char* file=malloc(sizeof(file)*100);
	scanf("%s",file);
	printf("%s\n",file);
	return file;
	free(file);
}
void printprofile(int id, char* file, char* sport){
	switch(id){
		case 1:
			printf("\tID: Sender\n");
			if(file!=NULL)
				printf("\tFile to send: %s\n", file);
			break;
		case 2: 
			printf("\tID: Receiver\n");
			if(file!=NULL)
				printf("\tFile to receive: %s\n", file);
			break;

	}
	if(sport!=NULL)
		printf("\tSerial port chosen: %s\n", sport);
	printf("\n\n\n\n");
}
char* serial(){
	printf("\tSerial ports available:\n");
	printf("\t1 for /dev/ttyS0\n");
	printf("\t2 for /dev/ttyS1\n");
	printf("\tChoose the serial port to exchange data: ");
	int portnr;
	scanf("%d",&portnr);
	while(portnr!=1 && portnr!=2){
		printf("\tError!! Choose a number (1 or 2) to exchange data: ");
		scanf("%d",&portnr);
	}
	switch(portnr){
		case 1: return "/dev/ttyS0";
		case 2: return "/dev/ttyS1";
	}
	return NULL;
}
void clearscreen() {
	printf("\033[2J");
} 
FILE* getOpen(char* filename) {
    
    FILE* file;
    
    if(appinfo.status == SENDER){
        file = fopen(filename, "rb");
        printf("\tOpened file (%s) as a sender with read parameters.\n", filename);
    }
    else{
        printf("\tSearching this file in current directory...\n");
        if(strcmp(searchcd(filename),"ok")!=0){
            printf("\tFile with name (%s) found in current directory!\n", filename);
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
                    printf("\n\tOK I stopped!\n");
                    exit(-1);
            }
        }
        //adicionar ver se existe um ficheiro com o mesmo nome neste diretorio
        file = fopen(filename, "wb");
        printf("\tCreated file (%s) as a receiver with write parameters.\n", filename);
    }
        
    if(file == NULL) {
        printf("\tError opening file requested (%s)\n", filename);
        return NULL;
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