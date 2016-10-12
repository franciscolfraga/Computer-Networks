#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include "exchange.h"

sfile aplication;
linklayer protocol;
void setup(int id, char* file, char* sport){
	/* open the device to be non-blocking (read will return immediatly) */
    int filed = open(sport, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (filed <0) {perror(sport); exit(-1); }
    /*serial port setup*/
    aplication.fd=filed;
    aplication.status=id;
    aplication.file=getOpen(file);
    protocol.port=sport;
    protocol.baudRate=4800;//change in main
    protocol.timeout=1;//change in main
    protocol.numTransmissions=3;//change in main opportunity to costumize
    printf("\tALL SET!\n");
	
}

FILE* getOpen(char* filename) {
    
    FILE* file;
    
    if(aplication.status == SENDER){
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
        printf("Error opening file requested (%s)\n", filename);
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