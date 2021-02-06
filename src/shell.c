#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h> 

#include "constants.h"
#include "structs.h"

// Prototypes
struct shell* initShell();
void toggleForegroundOnlyMode(int signo);

bool isForegroundOnlyMode = false; // Foreground only mode set as global variable since signal handling itself is a global operation

struct shell* initShell() {
	// Creates shell struct with default
	struct shell* shell = malloc(sizeof(struct shell));

	// Set all indexes equal to -1
	shell->backgroundPIDs = malloc(MAX_BACKGROUND_PROCESSES * sizeof(int));
	for (int i = 0; i < MAX_BACKGROUND_PROCESSES; i++) {
		shell->backgroundPIDs[i] = -1; // Since PIDs can range from 0 - 99999, set each empty index to -1
	}

	// Ignore SIGINT signal which is sent through ctrl+C
	shell->sigint.sa_handler = SIG_IGN;
	sigaction(SIGINT, &shell->sigint, NULL);

	// Use SIGTSTP (ctrl+Z) signal to toggle foreground only mode
	shell->sigtstp.sa_handler = toggleForegroundOnlyMode;
	sigfillset(&shell->sigtstp.sa_mask);
	shell->sigtstp.sa_flags = SA_RESTART;
	sigaction(SIGTSTP, &shell->sigtstp, NULL);

	shell->isForegroundOnlyMode = &isForegroundOnlyMode;

	return shell;
}

void toggleForegroundOnlyMode(int signo) {
	// Print relevant message, using write since printf is not reentrant
	if (isForegroundOnlyMode) {
		char* exitMsg = "\nExiting foreground-only mode\n";
		write(STDOUT_FILENO, exitMsg, 30);
		fflush(stdout);
	}
	else {
		char* enterMsg = "\nEntering foreground-only mode (& is now ignored)\n";
		write(STDOUT_FILENO, enterMsg, 50);
		fflush(stdout);
	}

	// Toggle isForegroundOnlyMode
	isForegroundOnlyMode = !isForegroundOnlyMode;

	// Print input prompt again
	printf(": ");
	fflush(stdout);
}