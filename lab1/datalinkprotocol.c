#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "settings.h"
#include "app.h"
#include "linkdata.h"

int main(){
	clearscreen();
	printf("\tThis file would run in the 2 machines (sender and receiver).\n");
	printf("\tLet's set you up.\n");
	int id= ID();
	char* file=NULL;
	char* sport=NULL;
	clearscreen();
	printprofile(id,file,sport);
	switch(id){
		case 1:
			file= sFile();
			break;
		case 2: 
			file= rFile();
			break;

	}
	clearscreen();
	printprofile(id,file,sport);
	sport=serial();
	clearscreen();
	printprofile(id,file,sport);
	setupapp(id,file,sport);
	//falta baudrate, nr de tentativas etc, vou dar uns de default
	setuplink(sport,appinfo.fd,id);
	return 0;
}