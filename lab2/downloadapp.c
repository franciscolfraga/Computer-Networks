
#include "downloadapp.h"
#include "connect.h"

url_info* info;
connection_info* firstconnection;
connection_info* secondconnection;
int main(int argc, char** argv){
	//ftp:ftp
	//ftp://[<user>:<password>@]<host>/<url-path>
	//allocate memory to the info
	info = (url_info*) malloc(sizeof(url_info));
	firstconnection =(connection_info*) malloc(sizeof(firstconnection));
	secondconnection =(connection_info*) malloc(sizeof(secondconnection));
	if(argc!=2)  {
		printf("Error! Only 2 args accepted!\n");
	}
	if(args_handler(argv[1])>=0)
		printf("Gathering info...\n");
	else{
		printf("Input not correct\n");
		exit(-1);
	}

	//send the webiste name
	char* ip=return_me_ip(info->hostname);
	info->hostip=malloc(sizeof(ip));
	strcpy(info->hostip,ip);
	int iport=21;
	info->port=iport;
	printf("Username: %s\n", info->nickname);
	printf("Password: %s\n", info->pw);
	printf("Host name: %s\n", info->hostname);
	printf("Host Ip: %s\n", info->hostip);
	printf("Host Port: %d\n", info->port);
	printf("Host filepath: %s\n", info->filepath);

	if((firstconnection->fd = start_connection(info->hostip,info->port)) == -1){
		printf("Can't connect socket!\n");
		exit(-1);
	}

	printf("All settings ok!\n");
	if(login()==-1){
		printf("Error in login! Exiting...\n");
		exit(-1);
	}
	printf("Logged in: true\n");
	if(passive()==-1){
		printf("Can't become passive! Exiting...\n");
		exit(-1);
	}
	printf("Passive: true\n");
	printf("Passive Ip: %s\n", secondconnection->ip);
	printf("Passive Port: %d\n", secondconnection->port);
	if((secondconnection->fd = start_connection(secondconnection->ip,secondconnection->port)) == -1){
		printf("Can't connect socket!\n");
		exit(-1);
	}
	if(get_path(info->filepath)<0){
		printf("Can't get path!\n");
		exit(-1);
	}
	lets_download(info->filepath);
	if(disconnect()<0){
		printf("Failed to disconnect\n");
		exit(-1);
	}
	printf("Disconnected\n\n\n");
	free(secondconnection);
	free(firstconnection);
	free(info);
	return 0;
}

char* return_me_ip(char* website_name){

	struct hostent* h;
	h = gethostbyname(website_name);

	char* ip=inet_ntoa(*((struct in_addr *) h->h_addr));
	return ip;
}

int args_handler(char* myarg){
	//check strlen later

	char* checkftp=strtok(myarg,":");
	if(strcmp(checkftp,"ftp")!=0){
		return -1;
	}
	char* checkuser=strtok(NULL,":");
	checkuser +=2;
	info->nickname=(char*)malloc(sizeof(checkuser));
	strcpy(info->nickname,checkuser);
	char* checkpw=strtok(NULL,"@");
	info->pw=(char*)malloc(sizeof(checkpw));
	strcpy(info->pw,checkpw);
	char* checkhost=strtok(NULL,"/");
	info->hostname=(char*)malloc(sizeof(checkhost));
	strcpy(info->hostname, checkhost);
	char* checkpath=strtok(NULL,"");
	printf("Checkpath: %s\n", checkpath);
	info->filepath=(char*)malloc(sizeof(checkpath));
	info->filepath=checkpath;
	return 0;
}
