#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "linkdata.h"
#include "alarm.h"
int buzz;
void handler(int signal) {
	if (signal != SIGALRM)
		return;

	buzz = 1;

	//add statistics

	printf("\t[Alarm] Timeout! Retrying...\n");
}

void setAlarm() {
	printf("\t[Alarm] Setting up alarm...\n");
	struct sigaction action;
	action.sa_handler = handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	sigaction(SIGALRM, &action, NULL);

	buzz = 0;

	alarm(linkinfo.timeout);
}

void stopAlarm() {
	struct sigaction action;
	action.sa_handler = NULL;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	sigaction(SIGALRM, &action, NULL);

	alarm(0);
}