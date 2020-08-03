#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "utils.h"

void print_proc_fd() {
	char cmd[100];
	sprintf(cmd, "ls -l /proc/%d/fd", getpid());
	if (system(cmd) != 0) {
		perror("system");
		exit(EXIT_FAILURE);
	}
}

void print_threads() {
	char cmd[100];
	sprintf(cmd, "ps -Lf -p %d", getpid());
	if (system(cmd) != 0) {
		perror("system");
		exit(EXIT_FAILURE);
	}
}