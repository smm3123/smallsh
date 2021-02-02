#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constant global variables
#define MAX_ARG_LENGTH 2048
#define MAX_ARGS 512

// Prototypes
enum programState;
enum programState handleInput();

enum programState {
	Okay,
	Failed
};

int main() {
	enum programState state = Okay;
	while (state == Okay) {
		printf(": ");
		state = handleInput();
	}
	return 0;
}

enum programState handleInput() {
	int input;
	scanf("%d", &input);
	if (input == 1)
		return Okay;
	else
		return Failed;
}