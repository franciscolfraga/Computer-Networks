
#include <stdio.h>
#include <stdlib.h>
#include "download.h"


url_info* info;

int main(int argc, char** argv){
	//ftp:ftp
	//ftp://[<user>:<password>@]<host>/<url-path>
	//allocate memory to the info
	info = (url_info*) malloc(sizeof(url_info));
	if(argc!=2)  {
		printf("Error! Only 2 args accepted!\n");
	}
	if(url_handler(argv[1])==1)
		printf("ok!\n");
	printf("%s\n", info->hostname);
	//send the webiste name
	char* ip=return_me_ip(info->hostname);
	info->hostip=malloc(sizeof(ip));
	strcpy(info->hostip,ip);
	printf("Ip: %s\n", info->hostip);

}

char* return_me_ip(char* website_name){

	struct hostent* h;
	printf("Url: %s\n",website_name);
	h = gethostbyname(website_name);

	char* ip=inet_ntoa(*((struct in_addr *) h->h_addr));
	return ip;
}

int url_handler(char* myarg){
	char* checkftp=strtok(myarg,":");
	printf("%s\n", checkftp);
	if(strcmp(checkftp,"ftp")!=0){
		printf("ftp needed\n");
		return 0;
	}
	char* checkuser=strtok(NULL,":");
	checkuser +=2;
	info->nickname=(char*)malloc(sizeof(checkuser));
	strcpy(info->nickname,checkuser);
	printf("%s\n", info->nickname);
	char* checkpw=strtok(NULL,"@");
	info->pw=(char*)malloc(sizeof(checkpw));
	strcpy(info->pw,checkpw);
	printf("%s\n", info->pw);
	char* checkhost=strtok(NULL,"/");
	info->hostname=(char*)malloc(sizeof(checkhost));
	strcpy(info->hostname, checkhost);
	printf("%s\n", info->hostname);
	return 1;
}
