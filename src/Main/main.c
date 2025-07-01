#include "TuringMachine/TuringMachine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERBOSE 'v'
#define STEP 's'
#define INPUT 'I'
#define OUTPUT 'O'
#define ONLINE_SYNTAX 'o'
#define CLEAR 'c'

// Prints the given turing machine in the online turing machine simulator
// syntax: https://turingmachinesimulator.com/
void toOnlineSimulatorSyntax(FILE* file, TuringMachine* tm) {
	
	// Print header
	fprintf(file, "name: [Give this TM a name]\ninit: %s\n", tm->rules->states[0]->name);
	
	// Print accept states
	char acceptStates[500];
	for (int i = 0; i < tm->rules->numStates; i++) {
		State* state = tm->rules->states[i];
		if (state->acceptState) {
			sprintf(acceptStates, "%s,", state->name);
		}
	}
	acceptStates[strlen(acceptStates) - 1] = '\n';
	fprintf(file, "accept: %s\n", acceptStates);

	// Print each file in the format
	for (int i = 0; i < tm->rules->numStates; i++) {
		State* state = tm->rules->states[i];
		for (int j = 0; j < state->numRules; j++) {
			Rule* rule = state->rules[j];
			char* stateToMoveTo = tm->rules->states[rule->stateToMoveTo]->name;
			char read = rule->readSymbol == BLANK ? '_' : rule->readSymbol;
			char write = rule->writeSymbol == BLANK ? '_' : rule->writeSymbol;
			fprintf(file, "%s,%c\n%s,%c,%c\n\n", state->name, read, stateToMoveTo, write, rule->moveDir);
		}
	}
}

// Takes the command line arguments and interprets them. Stores the options in the corresponding variables
// and stores the index of the input and output files in the locations pointed to by inputFile and outputFile
// Returns true if the inputs were valid, or returns false and prints an error message if invalid.
_Bool interpretCommandLineArgs(int argc, char** argv, _Bool* verbose, _Bool* step, _Bool* online, _Bool* clear, int* input, int* output) {
	
	const char ERROR_MSG[] = "Usage: TuringMachine [-vsco] -I input_file [tape_input] [-O output_file]";

	// Search for switches	
	for (int i = 1; i < argc; i++) {

		// If the input was a command line switch:
		if (argv[i][0] == '-') {
			
			// If the switch is empty, throw en error message
			int len = strlen(argv[i]);
			if (len == 1) {
				printf("%s\n", ERROR_MSG);
				printf("Error processing input: empty command line switch\n");
				return 0;
			}

			// Iterate over the string, ignoring the first character since it is '-'
			for (int j = 1; j < len; j++) {
				
				switch (argv[i][j]) {
					
					// If step, make sure that step is false. If it was already
					// set to true throw an error message
					case STEP:
					if (*step) {	
						printf("%s\n", ERROR_MSG);
						printf("Error processing input: step set twice\n");
						return 0;
					}
					else {
						*step = 1;
					}
					break;

					// If verbose, make sure that verbose is false. Throw error message if needed
					case VERBOSE:
					if (*verbose) {
						printf("%s\n", ERROR_MSG);
						printf("Error processing input: verbose set twice\n");
						return 0;
					}
					else {
						*verbose = 1;
					}
					break;
					
					// If this is the input, then it must be the only character in the input
					// The next string in the input array must be the input file
					case INPUT:
					if (*input != -1) {
						printf("%s\n", ERROR_MSG);
						printf("Error processing input: input is set twice\n");
						return 0;
					}
					else if (len > 2) {
						printf("%s\n", ERROR_MSG);
						printf("Error processing input: -I must not have any other switches in the same token\n");
						return 0;
					}
					else {
						*input = i + 1;
					}
					break;

					// Basically the same as input
					case OUTPUT:
					if (*output != -1) {
						printf("%s\n", ERROR_MSG);
						printf("Error processing input: output is set twice\n");
						return 0;
					}
					else if (len > 2) {
						printf("%s\n", ERROR_MSG);
						printf("Error processing input: -O must not have any other switches in the same token\n");
						return 0;
					}
					else {
						*output = i + 1;
					}
					break;

					// Just make sure that online syntax isn't already set
					case ONLINE_SYNTAX:
					if (*online) {
						printf("%s\n", ERROR_MSG);
						printf("Error processing input: online syntax switch is set twice\n");
						return 0;
					}
					else {
						*online = 1;
					}
					break;

					case CLEAR:
					if (*clear) {
						printf("%s\n", ERROR_MSG);
						printf("Error processing input: clear switch is set twice\n");
						return 0;
					}
					else {
						*clear = 1;
					}
					break;


					// Anything else is an invalid switch
					default:
						printf("%s\n", ERROR_MSG);
						printf("Error processing input: Unexpected input switch\n");
						return 0;
				}
			}
		}
	}
	
	// If no input file was provided, the input is invalid
	if (*input == -1) {
		printf("%s\n", ERROR_MSG);
		printf("Error processing input: no input file provided\n");
		return 0;
	}
	

	// If online syntax and verbose or step are set, the input is invalid
	if (*online && (*step || *verbose)) {
		printf("%s\n", ERROR_MSG);
		printf("Error processing input: Cannot have both -o and -v or -s\n");
		return 0;
	}

	// If output file is out of bounds or if input file is out of bounds,
	// the input is invalid
	if (*input >= argc) {
		printf("%s\n", ERROR_MSG);
		printf("Error processing input: -I must be followed by an input file\n");
		return 0;
	}
	if (*output >= argc) {
		printf("%s\n", ERROR_MSG);
		printf("Error processing input: -O must be followed by an output file\n");
		return 0;
	}

	// If -o is set and there is no output, the input is invalid
	if (*online && *output == -1) {
		printf("%s\n", ERROR_MSG);
		printf("Error processing input: Must provide an output file if -o is set\n");
		return 0;
	}

	// If -o is not set, then there must also be an input to the TM
	if (!*online && *input + 1 >= argc) {
		printf("%s\n", ERROR_MSG);
		printf("Error processing input: You must provide an input to the turing machine\n");
		return 0;
	}

	// If all above checks pass, input is valid
	return 1;
}

int main(int argc, char** argv) {
	
	_Bool verbose = 0;
	_Bool step = 0;
	_Bool online = 0;
	_Bool clear = 0;
	int input = -1;
	int output = -1;
	
	// Interpret command line input
	if (!interpretCommandLineArgs(argc, argv, &verbose, &step, &online, &clear, &input, &output)) {
		return 0;
	}

	TuringMachine* tm = makeTuringMachine();

	// Try to load from file
	const int MSG_LEN = 500;
	char msg[MSG_LEN];
	
	if (!loadTuringMachineFromFile(tm, argv[input], msg, MSG_LEN)) {
		printf("Input file was not valid: %s\n", msg);
		freeTuringMachine(tm);
		return 0;
	}
	
	// If online syntax switch was set, save TM and exit
	if (online) {
		FILE* file = fopen(argv[output], "w");
		if (file == NULL) {
			printf("%s could not be loaded.\n", argv[output]);
			freeTuringMachine(tm);
			return 0;
		}
		
		toOnlineSimulatorSyntax(file, tm);
		fclose(file);
		return 0;
	}

	// Execute the TM
	executeInput(tm, argv[input + 1], verbose, step, clear);
	
	// Clean up
	freeTuringMachine(tm);
	return 0;
}
