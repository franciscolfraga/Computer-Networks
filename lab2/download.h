#ifndef DOWNLOAD_H_   /* Include guard */
#define DOWNLOAD_H_

#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <libgen.h>


typedef struct Url {
	char* nickname; 
	char* pw;
	char* hostname;
	char* hostip;
	char* filepath; 
	char* filename;
} url_info;

extern url_info* info;

char* return_me_ip(char* website_name);

int url_handler(char* myarg);

#endif // DOWNLOAD_H_