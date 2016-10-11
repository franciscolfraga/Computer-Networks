#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "settings.h"

int main(){
	clearscreen();
	printf("\tThis file would run in the 2 machines (sender and receiver).\n");
	printf("\tLet's set up your settings\n");
	printf("\tPlease help me to identify you:\n\n");
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
	return 0;
}