#include "TuringMachine/TuringMachine.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// The max number of steps before the TM is halted
const int MAX_STEPS = 1000000;

// The max tape length before the TM is halted
const int MAX_TAPE = 1000;

// The default size of the char buffer used when loading from a file
const int LOADING_BUFFER_LEN = 256;

// The max size of an input file
const int MAX_SIZE = 1000;

const char CLEAR[] = "\e[2J\e[;H";

// Make a new TuringMachine
TuringMachine* makeTuringMachine() {
	TuringMachine* tm = (TuringMachine*) malloc(sizeof(TuringMachine));
	tm->state = 0;
	tm->steps = 0;
	tm->rules = makeRuleTree();
	tm->tape = NULL;
	return tm;
}

// Free the memory used by the turing machine
void freeTuringMachine(TuringMachine* tm) {
	freeRuleTree(tm->rules);
	free(tm);
}

// Loads an entire file into a buffer. This buffer MUST be freed by the caller
// fileLength is a pointer to where to store the amount of characters in the file
char* loadEntireFile(FILE* file, int* fileLength) {
	
	fseek(file, 0, SEEK_END);
	char* fileContents = (char*) malloc(ftell(file) + 1);
	fseek(file, 0, SEEK_SET);
	
	// Read file contents until we reach the end of the buffer
	char readChar = fgetc(file);
	int index = 0;
	while (readChar != EOF) {

		// Save read character into array
		fileContents[index] = readChar;
		index++;
		readChar = fgetc(file);
	}

	// Add a null terminator
	fileContents[index] = '\0';
	index++;

	// Save the index as the file length and return the contents
	*fileLength = index;
	return fileContents;
}

// Reads the main buffer and saves the next line in the array pointed to
// by the line argument. startIndex is the index of the first character in the
// line to read. Returns false when the file has been completely read (ie, a null terminator was found)
// Ignores comments (Anything after "//")
// Also trims any trailing whitespace
_Bool getLine(char* mainBuffer, int* startIndex, char* line, int lineSize) {
	int i = 0;
	char currentChar = mainBuffer[*startIndex];
	_Bool returnVal = 1;
	
	// Clear line
	for (int i = 0; i < lineSize; i++) {
		line[i] = '\0';
	}
	// Read while there is still space
	while (i < lineSize - 1) {
		
		// If the current char is a null terminator, stop reading and set return val
		if (currentChar == '\0') {
			returnVal = 0;
			break;
		}

		// If the current char is a newline, stop reading
		if (currentChar == '\n') {
			break;
		}

		// If this char is a slash and the next one, stop reading
		// Note that it's not possible to go out of bounds of the array here since, at worst,
		// the next character will be the null terminator
		if (currentChar == '/' && mainBuffer[*startIndex + i + 1] == '/') {
			// Decrement i by one since it's pointing at '/'
			if (i != 0) {
				i--;
			}
			break;
		}
		
		// Otherwise, copy the char into the line buffer
		line[i] = currentChar;

		i++;
		currentChar = mainBuffer[*startIndex + i];
	}

	// Set startIndex to be the character after the next newline character or next null terminator
	*startIndex = *startIndex + i;
	while (mainBuffer[*startIndex] != '\n' && mainBuffer[*startIndex] != '\0') {
		(*startIndex)++;
	}
	(*startIndex)++;

	// Trim off any trailing whitespace
	while(i >= 0) {
		if (line[i] != ' ' && line[i] != '\t' && line[i] != '\0') {
			break;
		}
		i--;
	}
	
	// Increment i by one since it's pointing at an important character
	i++;
	line[i] = '\0';
	return returnVal;
}

// Returns true if the given character is a-z, A-Z, or 0-9
_Bool isCharacter(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

// Processes a given line of input and determines if it's a state name.
// If so, the name is saved in the given name buffer. If the state is an 
// accept state, then accept is set to true.
// Returns true if the line is a state name
_Bool isStateName(char* line, char* name, _Bool* accept) {
	int index = 0;

	// Keep reading while there is content to read
	while (line[index] != '\0') {
		
		// Copy the name into the given buffer
		name[index] = line[index];

		// If we find a colon, then the state is likely a state name.
		// The colon can not be the first character
		if (line[index] == ':' && index != 0) {
			name[index] = '\0';
			
			// If the next character is a null terminator, then this is a state name
			// but not an accept state
			if (line[index + 1] == '\0') {
				*accept = 0;
				return 1;
			}

			// If the characters after are anything other than ACCEPT, then the state is invalid
			if (strcmp(&(line[index + 1]), " ACCEPT") == 0) {
				*accept = 1;
				return 1;
			}

			return 0;
		}

		// If the line contains any disallowed characters, then this is
		// not a name
		if (!isCharacter(line[index])) {
			return 0;
		}
		index++;
	}
	
	return 0;
}

// Loads a Turing Machine from a file. If the given file does not
// make a valid turing machine, the tm is set to NULL.
_Bool loadTuringMachineFromFile(TuringMachine* tm, char* fileName, char* errorMessage, int errorMessageSize) {
	
	if (tm == NULL) {
		char* error = "Failed to create blank turing machine";
		if (strlen(error) >= errorMessageSize - 1) {
			return 0;
		}
		strcpy(errorMessage, error);
		return 0;
	}

	// Open file
	FILE* file = fopen(fileName, "r");
	if (file == NULL) {
		char* error = "Failed to open file";
		if (strlen(error) >= errorMessageSize - 1) {
			return 0;
		}
		strcpy(errorMessage, error);
		return 0;
	}
	
	// Read the entire file
	int fileLength;
	char* buffer = loadEntireFile(file, &fileLength);

	fclose(file);

	// Now, add all states to the to the turing machine
	int i = 0;
	int line = 1;
	char lineBuffer[LOADING_BUFFER_LEN];
	while(getLine(buffer, &i, lineBuffer, LOADING_BUFFER_LEN)) {
		
		// If the input ends in a ":", then it is a state name.
		char name[LOADING_BUFFER_LEN];
		_Bool accept = 0;
		// If this is not a state name, ignore it and continue
		if (!isStateName(lineBuffer, name, &accept)) {
			line++;
			continue;
		}
		// Add the state. If it fails, the file is invalid
		if (!addNewState(tm->rules, name, accept)) {

			// Build error message
			char error[errorMessageSize];
			char lineNum[errorMessageSize];
			sprintf(lineNum, "%d", line);
			strcpy(error, "\nInvalid state on line ");
			strcat(error, lineNum);
			strcat(error, ": ");
			if (strlen(error) + strlen(name) >= errorMessageSize - 1) {
				free(buffer);
				return 0;
			}

			strcpy(errorMessage, error);
			strcat(errorMessage, name);
			free(buffer);
			return 0;
		}
		
		// Increment the line counter
		line++;
	}
	
	// Now to add the rules

	// This holds the name of the last state loaded
	char stateName[LOADING_BUFFER_LEN];
	_Bool dummyVar; // Dummy variable to feed to isStateName. Not used otherwise.
	line = 1;
	i = 0;
	while (getLine(buffer, &i, lineBuffer, LOADING_BUFFER_LEN)) {

		// If we find a state name, save that as the stateName
		char tempStateName[LOADING_BUFFER_LEN];
		if (isStateName(lineBuffer, tempStateName, &dummyVar)) {
			strcpy(stateName, tempStateName);
			line++;
			continue;
		}

		// If the line is empty, ignore it
		if (strlen(lineBuffer) == 0) {
			line++;
			continue;
		}
		
		// Since we are no longer looking at a state name, we must be looking at a rule.
		// The line for a rule should be "[Any amount of \t or \t][readSymbol] [writeSymbol] [move direction] [move to state name]"

		// Find first non-whitespace symbol
		int lineIndex;
		for (lineIndex = 0; lineIndex < strlen(lineBuffer); lineIndex++) {
			if (lineBuffer[lineIndex] != ' ' && lineBuffer[lineIndex] != '\t') {
				break;
			}
		}

		// Now, lineIndex should hold the readSymbol, lineIndex + 2 the writeSymbol, lineIndex + 4 the moveDirection, and the
		// name of the state to move to should start on lineIndex + 6. If lineIndex + 6 is out of bounds, then this line is not
		// a valid rule, and the file is invalid.

		// Additionally, lineIndex + 1, + 3, and + 5 must all be spaces, otherwise the line is invalid.
		if (lineIndex + 6 >= strlen(lineBuffer) ||
			lineBuffer[lineIndex + 1] != ' '    ||
			lineBuffer[lineIndex + 3] != ' '    ||
			lineBuffer[lineIndex + 5] != ' '    ) {

			char error[errorMessageSize];
			strcpy(error, "Unexpected line structure at line ");
			char errorLine[errorMessageSize];
			sprintf(errorLine, "%d", line);
			if (strlen(error) + strlen(errorLine) >= errorMessageSize - 1) {
				free(buffer);
				return 0;
			}
			strcpy(errorMessage, error);
			strcat(errorMessage, errorLine);
			free(buffer);
			return 0;
		}
		
		// Save the read, write, and direction characters
		char read = lineBuffer[lineIndex];
		char write = lineBuffer[lineIndex + 2];
		char dir = lineBuffer[lineIndex + 4];

		// Copy the name of the state to move to into a buffer
		char stateToMoveTo[LOADING_BUFFER_LEN];
		strcpy(stateToMoveTo, &(lineBuffer[lineIndex + 6]));
		
		// Replace read and write with null terminators if they are underscores
		if (read == '_') {
			read = BLANK;
		}
		if (write == '_') {
			write = BLANK;
		}

		// Try to make the rule. If it fails, the input is invalid
		const int ruleErrorSize = 200;
		char ruleError[ruleErrorSize];
		
		if (!addNewRule(tm->rules, stateName, read, write, dir, stateToMoveTo, ruleError, ruleErrorSize)) {
			// Constructing error message
			char error1[] = "\nError creating rule at line ";
			char errorLine[LOADING_BUFFER_LEN];
			sprintf(errorLine, "%d", line);
			char errorLineCont[] = ":\n";
			char error2[] = ": X X X ";
			error2[2] = (read == '\0') ? '_' : read;
			error2[4] = (write == '\0') ? '_' : write;
			error2[6] = dir;
			char* error3 = "\nReason: ";

			// Check that we have sufficient length in the error message buffer
			if (strlen(error1) + strlen(stateName) + strlen(errorLine) + strlen(errorLineCont) +
				strlen(error2) + strlen(stateToMoveTo) +
				strlen(error3) + strlen(ruleError) >= errorMessageSize - 1) {

				free(buffer);
				return 0;
			}
			
			// Construct error message
			strcpy(errorMessage, error1);
			strcat(errorMessage, errorLine);
			strcat(errorMessage, errorLineCont);
			strcat(errorMessage, stateName);
			strcat(errorMessage, error2);
			strcat(errorMessage, stateToMoveTo);
			strcat(errorMessage, error3);
			strcat(errorMessage, ruleError);
			free(buffer);
			return 0;
		}
		
		line++;
	}
	
	free(buffer);

	// If no states were loaded, then the file was either empty
	// or invalid
	if (tm->rules->numStates == 0) {
		char msg[] = "Provided file was empty";
		if (strlen(msg) >= errorMessageSize - 1) {
			return 0;
		}
		strcpy(errorMessage, msg);
		return 0;
	}
	return 1;
}


// Executes the given input and returns a string with the final tape
void executeInput(TuringMachine* tm, char* input, _Bool verbose, _Bool step, _Bool clear) {
	
	// Set up buffer for writing to console
	const int BUFFER_SIZE = (MAX_TAPE + 1) * 2;
	char buffer[BUFFER_SIZE];

	// Set up tape
	tm->tape = makeTape(input);

	// Get first rule and symbol
	char currentSymbol = get(tm->tape);
	Rule* currentRule = getRule(tm->rules, tm->state, currentSymbol);
	tm->steps = 0;
	
	// Continue until there are no more rules left, the tape gets too long.
	// or the max number of steps is reached
	while (currentRule != NULL && tm->steps <= MAX_STEPS && tm->tape->size <= MAX_TAPE) {
		
		if (clear) {
			printf("%s", CLEAR);
		}
		if (verbose) {
			tapeToString(tm->tape, buffer, BUFFER_SIZE);
			printf("Steps: %d\n", tm->steps);
			printf("State: %s\n", tm->rules->states[tm->state]->name);
			printf("%s\n\n", buffer);
			if (step) {
				printf("Press enter to advance to next step\n");
				char dummyInput;
				scanf("%c", &dummyInput);
			}
		}
		
		// Write the symbol
		write(tm->tape, currentRule->writeSymbol);

		// Move the head and get the next symbol
		char dir = currentRule->moveDir;
		if (dir == '<') {

			currentSymbol = previous(tm->tape);
		}
		else {
			currentSymbol = next(tm->tape);
		}

		// Move to the appropriate next state
		tm->state = currentRule->stateToMoveTo;
	
		// Get the next rule and increment steps counter
		currentRule = getRule(tm->rules, tm->state, currentSymbol);
		tm->steps++;
	}

	// Display final state
	if (clear) {
		printf("%s", CLEAR);
	}
	tapeToString(tm->tape, buffer, BUFFER_SIZE);
	if (tm->steps > MAX_STEPS || tm->tape->size > MAX_TAPE) {
		printf("\nNEITHER ACCEPT NOR REJECT\n");
	}
	else if (isAcceptState(tm->rules, tm->state)) {
		printf("\nACCEPT\n");
	}
	else {
		printf("\nREJECT\n");
	}

	printf("Steps: %d\n", tm->steps);
	printf("State: %s\n", tm->rules->states[tm->state]->name);
	printf("%s\n\n", buffer);

	// Clean up
	freeTape(tm->tape);
}
