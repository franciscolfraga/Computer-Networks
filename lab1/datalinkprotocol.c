#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "link_layer.h"
#include "app_layer.h"


int main()
{
    clrscr();

	printf("\tThis file would run in the 2 machines (sender and receiver).\n");
	printf("\tLet's set you up.\n\n\n");
	int id, packet_size=-1, baudrate=-1, nr_retries=-1, timeout=-1;

	char* port=NULL;

	char* file_name=NULL;

	id = get_id();
	if( id == RECEIVER || id == SENDER)
		;
	else
		return ERROR;

	clrscr();

	printprofile(id, packet_size, baudrate, nr_retries, timeout, port, file_name);

	packet_size = get_packet_size();

	clrscr();

	printprofile(id, packet_size, baudrate, nr_retries, timeout, port, file_name);

	baudrate = get_baudrate();

	clrscr();

	printprofile(id, packet_size, baudrate, nr_retries, timeout, port, file_name);

	nr_retries = get_retries();

	clrscr();

	printprofile(id, packet_size, baudrate, nr_retries, timeout, port, file_name);

	timeout = get_timeout();


	clrscr();

	printprofile(id, packet_size, baudrate, nr_retries, timeout, port, file_name);
	
	port = get_serial_port();
	
	clrscr();

	printprofile(id, packet_size, baudrate, nr_retries, timeout, port, file_name);

    file_name = get_file_name(id);

    clrscr();

    printprofile(id, packet_size, baudrate, nr_retries, timeout, port, file_name);

    get_app(port, id, file_name,timeout, nr_retries, packet_size, baudrate);

   	return 0;
}

