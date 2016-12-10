#include "downloadapp.h"
#include "connect.h"

int disconnect() {
	
	char* dc = malloc(6 * sizeof(char));
	
	sprintf(dc, "quit\r\n");
	
	if (sending_socket(firstconnection->fd, dc) == -1) {
		return -1;
	}

	if (firstconnection->fd) {
		close(firstconnection->fd);
	}
	

	return 0;
}


int start_connection(char* ip, int port) {

	struct sockaddr_in server_addr;
	bzero((char*)&(server_addr),sizeof((server_addr)));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);	
	server_addr.sin_port = htons(port);

	int fd;
	if ((fd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    		perror("Could not execute socket()\n");
        	exit(-1);
    }

    printf("Socket created: true\n");
    if(check_socket(fd)==-1){
    	printf("Socket is not good.\n");
    	exit(-1);
    }

    if (connect(fd, (struct sockaddr *)&(server_addr), sizeof(server_addr)) < 0){
        perror("Could not execute connect()\n");
		exit(-1);
	}

	printf("Socket fd: %d\n",fd);

	if (port == 21) {

		char * msg = malloc(5 * sizeof(char));


		if (reading_socket(fd, msg,"220") == -1) {
			printf("Invalid connect message!\n");
			return -1;
		}

		printf("Socket receiving messages: true\n");
		
		free(msg);

	}

	return fd;
}

int reading_socket(int fd, char* msg, char* code) {

	FILE* file_to_read = fdopen(fd, "r");
	int size = 4;
	printf("Reading from socket: ");
	while (!('1' <= msg[0] && msg[0] <= '5')){
		memset(msg, 0, size);
		msg = fgets(msg, size, file_to_read);
		
		printf("%s\n", msg);

	}



	if (strcmp(msg, code) != 0) {	
		return -1;
	}
	return 0;
}

int check_socket(int socket_fd){
	int error = 0;
	socklen_t len = sizeof (error);
	int retval = getsockopt (socket_fd, SOL_SOCKET, SO_ERROR, &error, &len);
	if (retval != 0) {
	    /* there was a problem getting the error code */
	    fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
	    return -1;
	}

	if (error != 0) {
	    /* socket has a non zero error status */
	    fprintf(stderr, "socket error: %s\n", strerror(error));
	    return -1;
	}
	return 0;	
}

int login() {

	char* user = malloc(sizeof(info->nickname) + 5 * sizeof(char));
	sprintf(user, "user %s\r\n", info->nickname);
	char * password = malloc(sizeof(info->pw) + 5 * sizeof(char));
	sprintf(password, "pass %s\r\n", info->pw);

	if (sending_socket(firstconnection->fd, user) == -1) {
		printf("Error sending a message to host.");
		return -1;
	} 


	if (reading_socket(firstconnection->fd, user, "331") == -1) {
		printf("Not a valid user message!\n");
		return -1;
	}


	if (sending_socket(firstconnection->fd, password) == -1) {
		printf("Error sending a message to host.\n");
		return -1;
	} 


	if (reading_socket(firstconnection->fd, password, "230") == -1) {
		printf("Log in failed!\n");
		return -1;
	}
	free(user);
	free(password);



	return 0;
}

int sending_socket(int firstconnection_fd, const char* msg) {
	
	int write_bytes = 0;

	write_bytes = write(firstconnection_fd, msg, strlen(msg));

	int value = (write_bytes == strlen(msg)) ? 0 : -1; 

	return value;

}

int get_passive(char* string_ip) {
	FILE* file_to_read = fdopen(firstconnection->fd, "r");
	int size = 1024;
	char* msg = malloc(size * sizeof(char));
	printf("Passive message is: ");
	while (!('1' <= msg[0] && msg[0] <= '5')) {
		memset(msg, 0, size);
		msg = fgets(msg, size, file_to_read);
		printf("%s", msg);
	}
	int ip[4];
	int port_arr[2];

	if ((sscanf(msg, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", 
		&ip[0], &ip[1], &ip[2], &ip[3], &port_arr[0], &port_arr[1])) < 0)
		return -1;

	if (sprintf(string_ip, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]) < 0)
		return -1;

	secondconnection->port = 256 * port_arr[0] + port_arr[1];


	return 1;
}

int passive() {

	char * pasv = malloc(7 * sizeof(char));
	sprintf(pasv, "pasv \r\n");
	if (sending_socket(firstconnection->fd, pasv) == -1) {
		printf("\t->Error sending a message to host.");
		return -1;
	} 


	char* ip = malloc(50 * sizeof(char));

	if( get_passive( ip ) < 0 ) {
		printf("\t->Error interpreting passive message.\n");
		return -1;
	}

	char* ip_info = malloc(1024 * sizeof(char));

	strcpy(ip_info, "Interpreted IP: \0");
	strcat(ip_info, ip);


	char* port_info = malloc(1024 * sizeof(char));

	if (sprintf(port_info, "Interpreted Port: %d", secondconnection->port) < 0)  {
		printf("\t->Error printing port to string.\n");
		return -1;
	}

	secondconnection->ip = ip;


	return 0;
}

int get_path(char * path) {	
	
	char * retr = malloc(1024 * sizeof(char));
	sprintf(retr, "retr %s\r\n", path);

	char * path_info = malloc(1024 * sizeof(char));
	sprintf(path_info, "File: %s", path);


	if (sending_socket(firstconnection->fd, retr) == -1) {
		return -1;
	} 


	if (reading_socket(firstconnection->fd, retr, "150") == -1) {
		return -1;
	}

	free(retr);
	free(path_info);	
	return 0;
}


int lets_download( char* path) {
	FILE* file_to_read;
	int bytes;
	char* filename = basename(path);
	if (!(file_to_read = fopen(filename, "w"))) {
		printf("Cannot open file.\n");
		return -1;
	}
	int acumulator=0;
	printf("File created\n");
	printf("Downloading...\n");
	char* buf = malloc(1024);
	while ((bytes = read(secondconnection->fd, buf, sizeof(buf)))) {
		if (bytes < 0) {
			printf("Nothing was received from data socket fd.\n");
			return -1;
		}

		if ((bytes = fwrite(buf, bytes, 1, file_to_read)) < 0) {
			printf("Cannot write data in file.\n");
			return -1;
		}
		acumulator+=bytes;
		printf("%d bytes\n",acumulator);

	}
	
	if(secondconnection->fd) {
		close(secondconnection->fd);
		free(secondconnection);
	}

	if(file_to_read){
	 	fclose(file_to_read);
	 }
	return 0;
}
