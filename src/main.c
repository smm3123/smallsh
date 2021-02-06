#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "constants.h"
#include "structs.h"
#include "background_processes.h"
#include "shell.c"
#include "command.c"

int main() {
	enum programState state = Okay;
	int terminationStatus = 0;
	struct shell* shell = initShell();

	while (state == Okay) {
		// Print and flush output buffer
		printf(": ");
		fflush(stdout);

		// Get user input and parse arguments
		char userInput[MAX_CMD_LENGTH];
		getArguments(userInput);
		struct command cmd = parseArgumentsAndGetCommand(userInput, *shell->isForegroundOnlyMode);

		// Execute the command and arguments
		state = executeCommand(cmd, &terminationStatus, shell);

		// Check to see if background processes have finished running
		checkBackgroundProcessStatus(shell->backgroundPIDs);
	}

	return 0;
}

