#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>

// Structs & enums
enum programState {
	Okay,
	Exit
};

// Represents the shell state
struct shell {
	struct sigaction sigint;
	struct sigaction sigtstp;
	int* backgroundPIDs; // Will be array of size MAX_BACKGROUND_PROCESSES
	bool* isForegroundOnlyMode;
};

struct redirectFlag {
	bool isInArgument;
	int argumentPosition; // Index within the arguments array that the redirect is at
};

// Represents the command the user inputs
struct command {
	char** arguments;
	struct redirectFlag inputFlag;
	struct redirectFlag outputFlag;
	bool isBackgroundProcess;
};

#endif