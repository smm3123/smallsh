#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>

// Constant global variables
#define MAX_CMD_LENGTH 2048
#define MAX_ARGS 512

// Prototypes
enum programState;
struct redirectFlag;
struct command;
void getArguments(char* userInput);
struct command parseArgumentsAndGetCommand(char* args);
void replaceDollarSignsWithPID(char* str);
enum programState executeCommand(struct command cmd, int* terminationStatus);
void executeCd(char** args);
void executeStatus(int status);
void executeNonBuiltInCommand(struct command cmd, int* termninationStatus);
void handleIORedirection(struct command cmd);
void handleRedirection(struct command cmd, int oflag, int newFileDescriptor, bool isInput);

// Structs & enums
enum programState {
	Okay,
	Exit
};

struct redirectFlag {
	bool isInArgument;
	int argumentPosition; // Index within the arguments array that the redirect is at
};

struct command {
	char** arguments;
	struct redirectFlag inputFlag;
	struct redirectFlag outputFlag;
	bool isBackgroundProcess;
};

int main() {
	enum programState state = Okay;
	int terminationStatus;
	while (state == Okay) {
		// Print and flush output buffer
		printf(": ");
		fflush(stdout);

		// Get user input and parse arguments
		char userInput[MAX_CMD_LENGTH];
		getArguments(userInput);
		struct command cmd = parseArgumentsAndGetCommand(userInput);

		// Execute the command and arguments
		state = executeCommand(cmd, &terminationStatus);
	}
	return 0;
}

void getArguments(char* userInput) {
	// Get input and remove trailing new line
	fgets(userInput, MAX_CMD_LENGTH, stdin);
	userInput[strlen(userInput) - 1] = '\0';
}

struct command parseArgumentsAndGetCommand(char* args) {
	// Stores each argument passed in by the user in an array index and returns the array
	char** argsArray = malloc(MAX_CMD_LENGTH * sizeof(char*));
	char* savePtr;
	int counter = 0;

	// Initialize command
	struct command cmd;
	cmd.inputFlag.isInArgument = false;
	cmd.outputFlag.isInArgument = false;
	cmd.isBackgroundProcess = false;

	// Parse arguments into argsArray
	char* token = strtok_r(args, " ", &savePtr);
	while (token != NULL) {
		argsArray[counter] = token;

		// Check if input or output arguments were passed to the command
		if (strcmp(token, ">") == 0) {
			cmd.outputFlag.isInArgument = true;
			cmd.outputFlag.argumentPosition = counter;
		}
		else if (strcmp(token, "<") == 0) {
			cmd.inputFlag.isInArgument = true;
			cmd.inputFlag.argumentPosition = counter;
		}
		// Check and replace $$ within the token with PID
		else if (strstr(token, "$$") != NULL) {
			char temp[1024];
			strcpy(temp, token);
			replaceDollarSignsWithPID(temp);
			argsArray[counter] = temp; // Replace token with new string
		}

		counter++;
		token = strtok_r(NULL, " ", &savePtr);
	}

	// Check if command specifies if it's a background process
	if (strcmp(argsArray[counter - 1], "&") == 0) {
		cmd.isBackgroundProcess = true;
		argsArray[counter - 1] = NULL; // Remove ampersand so it isn't passed as an argument
	}

	argsArray[counter] = NULL; // Last array index will be NULL so an empty argument isn't passed
	cmd.arguments = argsArray;
	free(argsArray);
	return cmd;
}

void replaceDollarSignsWithPID(char* str) {
	// Variable expansion functionality
	// Replace all instances of $$ with process ID and return result
	char buffer[1024];
	char needle[3] = "$$\0";
	char pid[16];
	char* p;

	sprintf(pid, "%d", getpid()); // Convert pid to a string

	while ((p = strstr(str, needle))) {
		strncpy(buffer, str, p - str);
		buffer[p - str] = '\0';
		strcat(buffer, pid);
		strcat(buffer, p + strlen(needle));
		strcpy(str, buffer);
		p++;
	}
}

enum programState executeCommand(struct command cmd, int* terminationStatus) {
	// Checks the command the user inputted and handles what to execute accordingly

	// Check if argument is null or comment
	// If the first character in the arg is '#', it's considered a comment - midline comments not within scope of project
	if ((cmd.arguments[0] == NULL || strcmp(cmd.arguments[0], "") == 0) || cmd.arguments[0][0] == '#')
		return Okay;
	// exit built-in command
	else if (strcmp(cmd.arguments[0], "exit") == 0)
		return Exit;
	// cd built-in command
	else if (strcmp(cmd.arguments[0], "cd") == 0) {
		executeCd(cmd.arguments);
		return Okay;
	}
	// status built-in command
	else if (strcmp(cmd.arguments[0], "status") == 0) {
		executeStatus(*terminationStatus);
	}
	else {
		executeNonBuiltInCommand(cmd, terminationStatus);
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

void executeStatus(int status) {
	if (WIFEXITED(status)) {
		printf("exit value %d\n", WEXITSTATUS(status));
		fflush(stdout);
	}
}

void executeNonBuiltInCommand(struct command cmd, int* terminationStatus) {
	// Fork new process
	pid_t spawnPid = fork(); 
	switch (spawnPid) {
		case -1:
			// Error creating new process
			perror("fork()\n");
			exit(1);
			break;
		case 0:
			// In the child process
			handleIORedirection(cmd);
			execvp(cmd.arguments[0], cmd.arguments); // Use execvp per suggestion from instructions
			perror("execv"); // exec will only return if there's an error
			exit(2);
			break;
		default:
			// In the parent process
			spawnPid = waitpid(spawnPid, terminationStatus, 0);
			break;
	}
}

void handleIORedirection(struct command cmd) {
	// Handle input redirection
	if (cmd.inputFlag.isInArgument)
		handleRedirection(cmd, O_RDONLY, STDIN_FILENO, true);

	// Handle output redirection
	if (cmd.outputFlag.isInArgument)
		handleRedirection(cmd, O_WRONLY | O_CREAT | O_TRUNC, STDOUT_FILENO, false);
}

void handleRedirection(struct command cmd, int oflag, int newFileDescriptor, bool isInput) {
	int index = isInput ? cmd.inputFlag.argumentPosition : cmd.outputFlag.argumentPosition;
	int fileDescriptor = open(cmd.arguments[index + 1], oflag, 0640);
	if (fileDescriptor == -1) {
		printf("cannot %s %s for %s\n", isInput ? "open" : "create", cmd.arguments[index + 1], isInput ? "input" : "output");
		fflush(stdout);
		exit(2);
	}
	else {
		int result = dup2(fileDescriptor, newFileDescriptor);
		if (result == -1) {
			perror("dup2");
			exit(2);
		}
		cmd.arguments[index] = NULL; // Prevent the redirect flag from being passed as an argument during command execution
		close(fileDescriptor);
	}
}