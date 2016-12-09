#ifndef DOWNLOADAPP_H_   /* Include guard */
#define DOWNLOADAPP_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <libgen.h>

#define ERROR -1


typedef struct Url {
	char* nickname; 
	char* pw;
	char* hostname;
	char* hostip;
	char* filepath; 
	char* filename;
	int port;
} url_info;

extern url_info* info;
char* return_me_ip(char* website_name);

int args_handler(char* myarg);

#endif // DOWNLOADAPP_H_
