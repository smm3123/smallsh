#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>

// Constant global variables
#define MAX_ARG_LENGTH 2048
#define MAX_ARGS 512

// Prototypes
enum programState;
void getArguments(char* userInput);
char** parseArguments(char* args);
enum programState executeInput(char** args);
void executeCd(char** args);
void executeNonBuiltInCommand(char** args);

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

		// Execute the command and arguments
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
	// Stores each argument passed in by the user in an array index and returns the array
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
	// Checks the command the user inputted and handles what to execute accordingly

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
	else {
		executeNonBuiltInCommand(args);
		return Okay;
	}
}

void executeCd(char** args) {
	// Handle the cd built in command
	// Change dir to HOME environment variable if user doesn't specify directory
	char* dir = (args[1] == NULL) ? getenv("HOME") : args[1];
	int chdirStatus = chdir(dir);

	if (chdirStatus != 0) {
		printf("Directory not found: %s\n", dir);
		fflush(stdout);
	}
}

void executeNonBuiltInCommand(char** args) {
	// Fork new process
	int childStatus;
	pid_t spawnPid = fork(); 
	switch (spawnPid) {
		case -1:
			// Error creating new process
			perror("fork()\n");
			exit(1);
			break;
		case 0:
			// In the child process
			execvp(args[0], args); // Use execvp per suggestion from instructions
			perror("execv"); // exec will only return if there's an error
			exit(2);
			break;
		default:
			// In the parent process
			spawnPid = waitpid(spawnPid, &childStatus, 0);
			break;
	}
}