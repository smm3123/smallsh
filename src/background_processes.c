#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include "constants.h"

void checkBackgroundProcessStatus(int backgroundPIDs[]) {
	// Checks if any background processes are still running
	int status;
	int pid = 0;

	// Keep looping until there are no more background processes that have ended
	do {
		pid = waitpid(-1, &status, WNOHANG); // WNOHANG makes waitpid non-blocking
		// If background process has ended
		if (pid > 0) {
			// Background process exited
			if (WIFEXITED(status)) {
				printf("background pid %d is done: exit value %d\n", pid, WEXITSTATUS(status));
				fflush(stdout);
			}
			// Background process terminated
			else {
				printf("background pid %d is done: terminated by signal %d\n", pid, WTERMSIG(status));
			}
			removeBackgroundPID(backgroundPIDs, pid);
		}
	} while (pid > 0);
}

void addBackgroundPID(int backgroundPIDs[], int pid) {
	// Add background PID to first empty index in the array
	for (int i = 0; i < MAX_BACKGROUND_PROCESSES; i++) {
		if (backgroundPIDs[i] == -1) {
			backgroundPIDs[i] = pid;
			break;
		}
	}
}

void removeBackgroundPID(int backgroundPIDs[], int pid) {
	// Replace PID with -1 to indicate empty index
	for (int i = 0; i < MAX_BACKGROUND_PROCESSES; i++) {
		if (backgroundPIDs[i] == pid) {
			backgroundPIDs[i] = -1;
			break;
		}
	}
}

void killBackgroundProcesses(int backgroundPIDs[]) {
	// Iterate through background processes and kill any that are still running
	for (int i = 0; i < MAX_BACKGROUND_PROCESSES; i++) {
		if (backgroundPIDs[i] != -1) {
			kill(backgroundPIDs[i], SIGTERM); // Terminate the process
			free(&backgroundPIDs[i]); // Free from memory
		}
	}
}