#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

// Constant global variables
#define MAX_ARG_LENGTH 2048
#define MAX_ARGS 512

// Prototypes
enum programState;
void getArguments(char* userInput);
char** parseArguments(char* args);
enum programState executeInput(char** args);
void executeCd(char** args);

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

		// Get user input and parse arguments
		char userInput[MAX_ARG_LENGTH];
		getArguments(userInput);
		char** args = parseArguments(userInput);

		state = executeInput(args);
	}
	return 0;
}

void getArguments(char* userInput) {
	// Get input and remove trailing new line
	fgets(userInput, MAX_ARG_LENGTH, stdin);
	userInput[strlen(userInput) - 1] = '\0';
}

char** parseArguments(char* args) {
	char** argsArray = malloc(MAX_ARG_LENGTH * sizeof(char*));
	char* savePtr;
	int counter = 0;

	char* token = strtok_r(args, " ", &savePtr);
	while (token != NULL) {
		argsArray[counter] = token;
		counter++;
		token = strtok_r(NULL, " ", &savePtr);
	}

	argsArray[counter] = NULL; // Last array index will be NULL for iteration purposes
	return argsArray;
}

enum programState executeInput(char** args) {
	// Check if argument is null or comment
	// If the first character in the arg is '#', it's considered a comment - midline comments not within scope of project
	if ((args[0] == NULL || strcmp(args[0], "") == 0) || args[0][0] == '#')
		return Okay;
	// exit built-in command
	else if (strcmp(args[0], "exit") == 0)
		return Exit;
	// cd built-in command
	else if (strcmp(args[0], "cd") == 0) {
		executeCd(args);
		return Okay;
	}
	else
		return Okay;
}

void executeCd(char** args) {
	// Handle the cd built in command
	// Change dir to HOME environment variable if user doesn't specify directory
	int chdirStatus = (args[1] == NULL) ? chdir(getenv("HOME")) : chdir(args[1]);

	if (chdirStatus != 0) {
		printf("Directory not found: %s\n", (args[1] == NULL) ? getenv("HOME") : args[1]);
		fflush(stdout);
	}
}