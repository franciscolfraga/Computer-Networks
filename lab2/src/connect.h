#ifndef CONNECT_H_   /* Include guard */
#define CONNECT_H_

typedef struct Connection {
	int port;
	int fd;
	char* ip;
} connection_info;
extern connection_info* firstconnection;
extern connection_info* secondconnection;
int disconnect();
int start_connection(char * ip, int port);
int sending_socket(int connection_fd, const char* msg);
int reading_socket(int fd, char* msg, char* code);
int check_socket(int socket_fd);
int login();
int passive();
int get_passive(char* ip);
int get_path(char * path);
int lets_download( char* path);
FILE* file_to_read;

#endif // DOWNLOADAPP_H_