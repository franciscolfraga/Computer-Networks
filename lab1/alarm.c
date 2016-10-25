#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "link_layer.h"

int buzz = 0;

void handler(int signal) {
	if (signal != SIGALRM)
		return;

	link_info->stats_info.timeout++;

	buzz = 1;

	printf("Timeout! Retrying...\n");
}

void setAlarm() {
	struct sigaction action;

	action.sa_handler = handler;

	sigemptyset(&action.sa_mask);

	action.sa_flags = 0;

	buzz = 0;

	sigaction(SIGALRM, &action, NULL);

	alarm(link_info->timeout);
}

void stopAlarm() {
	struct sigaction action;

	action.sa_handler = NULL;

	sigemptyset(&action.sa_mask);
	
	action.sa_flags = 0;

	sigaction(SIGALRM, &action, NULL);

	alarm(0);
}
