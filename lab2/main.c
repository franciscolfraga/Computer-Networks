
#include <stdio.h>
#include <stdlib.h>
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
	char* host;
	char* filepath; 
	char* filename;
} url;

void url_handler(char* my_url);

int main(int argc, char** argv){
	//ftp:ftp
	if(argc!=2)  {
		printf("Error! Only 2 args accepted!\n");
	}
	//send the url
	url_handler(argv[1]);
}

void url_handler(char* my_url){
	struct hostent* h;
	printf("Url: %s\n",my_url);
	h = gethostbyname(my_url);
	char* ip = inet_ntoa(*((struct in_addr *) h->h_addr));
	printf("Ip: %s\n", ip);
}

