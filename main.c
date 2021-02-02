#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constant global variables
#define MAX_ARG_LENGTH 2048
#define MAX_ARGS 512

// Prototypes
void getArguments(char* args);
enum programState;
enum programState executeInput(char* args);

enum programState {
	Okay,
	Exit
};

int main() {
	enum programState state = Okay;
	while (state == Okay) {
		// Print and flush output buffer
		printf(": ");
		fflush(stdout);

		// Get user input
		char args[MAX_ARG_LENGTH];
		getArguments(args);

		state = executeInput(args);
	}
	return 0;
}

void getArguments(char* args) {
	// Get input and remove trailing new line
	fgets(args, MAX_ARG_LENGTH, stdin);
	args[strlen(args) - 1] = '\0';
}

enum programState executeInput(char* args) {
	// Check if argument is null or comment
	// If the first character in the arg is '#', it's considered a comment - midline comments not within scope of project
	if ((args == NULL || strcmp(args, "") == 0) || args[0] == '#')
		return Okay;
	if (strcmp(args, "exit") == 0)
		return Exit;
	else
		return Okay;
}