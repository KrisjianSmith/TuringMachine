#include "TuringMachine/Tape.h"
#include <stdlib.h>

const char BLANK = '\0';

// Make new tapeNode with a blank symbol
TapeNode* makeTapeNode(TapeNode* prev, TapeNode* next) {
	TapeNode* returnVal = (TapeNode*) malloc(sizeof(TapeNode));
	returnVal->symbol = BLANK;
	returnVal->next = next;
	returnVal->prev = prev;
	return returnVal;
}

void freeTapeNode(TapeNode* tapeNode) {
	
	// Make sure nothing is lost
	if (tapeNode->prev != NULL) {
		tapeNode->prev->next = tapeNode->next;
	}
	if (tapeNode->next != NULL) {
		tapeNode->next->prev = tapeNode->prev;
	}

	// Now that previous and next are pointing at each other,
	// delete the tapeNode
	free(tapeNode);
}

// Frees all the tape nodes
void freeAllTapeNodes(TapeNode* tape) {
	// go to the very first node
	TapeNode* temp = tape;
	while (temp->prev != NULL) {
		temp = temp->prev;
	}

	// Move forward one node and free the previous
	while (temp->next != NULL) {
		temp = temp->next;
		free(temp->prev);
	}

	// Now that there is only one node left, free it
	free(temp);
}

// Make new tape
Tape* makeTape(char* input) {
	
	Tape* returnVal = (Tape*)malloc(sizeof(Tape)); 
	returnVal->size = 1;
	
	// Set current TapeNode to a new, blank tape
	returnVal->current = makeTapeNode(NULL, NULL);
	
	// Load input onto tape
	// Save first symbol so we don't have to iterate all the way back
	TapeNode* firstSymbol = returnVal->current;

	int index = 0;
	while (input[index] != '\0') {
		write(returnVal, input[index]);
		next(returnVal);
		index++;
	}
	// Go to the previous node to get rid of the last empty node
	if (returnVal->current->prev != NULL) {
		previous(returnVal);
	}

	// Restore current to first symbol
	returnVal->current = firstSymbol;

	return returnVal;
}

// Free memory used by tape
void freeTape(Tape* tape) {
	
	freeAllTapeNodes(tape->current);
	free(tape);
}

// Get the next character on the tape
char next(Tape* tape) {
	
	// If there is no next symbol, make one and increment the size
	if (tape->current->next == NULL) {
		tape->current->next = makeTapeNode(tape->current, NULL);
		tape->size++;
	}

	// Set the current symbol to the next symbol
	tape->current = tape->current->next;
	
	// If the symbol we just came from was blank and was the last symbol on the tape,
	// delete it to save space
	if (tape->current->prev->symbol == BLANK &&
		tape->current->prev->prev == NULL) {

		freeTapeNode(tape->current->prev);
		tape->current->prev = NULL;
		tape->size--;
	}

	return tape->current->symbol;
}

// Get the previous character on the tape
char previous(Tape* tape) {
	// If there is no previous symbol, make one and increment the size
	if (tape->current->prev == NULL) {
		tape->current->prev = makeTapeNode(NULL, tape->current);
		tape->size++;
	}
	
	// Move to previous spot on the tape
	tape->current = tape->current->prev;
	// If the symbol we just came from was blank and was the first symbol on the tape,
	// delete it to save space
	if (tape->current->next->symbol == BLANK &&
		tape->current->next->next == NULL) {

		freeTapeNode(tape->current->next);
		tape->current->next = NULL;
		tape->size--;
	}

	// Set the current symbol to the next symbol
	return tape->current->symbol;
}

// Get the current character on the tape
char get(Tape* tape) {
	return tape->current->symbol;
}

// Write the given character to the tape at the current position
void write(Tape* tape, char c) {
	tape->current->symbol = c;
}

// Turn tape into String of characters
void tapeToString(Tape* tape, char* buffer, int bufferSize) {
	
	
	// Make two Strings that we will fill the buffer with.
	// It is one more than the size of the tape to accomidate for the
	// null pointer and newline
	char* upper = (char*) malloc (sizeof(char) * (tape->size + 1));
	char* lower = (char*) malloc (sizeof(char) * (tape->size + 1));

	// Find the first TapeNode in the list
	TapeNode* temp = tape->current;
	while (temp->prev != NULL) {
		temp = temp->prev;
	}

	// Get each symbol on the tape. If it is a blank, print _
	for (int i = 0; i < tape->size && temp != NULL; i++) {
		char curChar = temp->symbol;
		if (curChar == BLANK) {
			upper[i] = '_';
		}
		else {
			upper[i] = curChar;
		}

		// Test to see if this is the current place that the tape is pointing
		if (temp == tape->current) {
			// If so, print a ^ on the lower line of the output
			lower[i] = '^';
		}
		else {
			// Otherwise, put a space
			lower[i] = ' ';
		}

		// Go to next tape symbol
		temp = temp->next;
	}

	// Put the null terminator and newline symbols
	upper[tape->size] = '\n';
	lower[tape->size] = '\0';

	// Fill the buffer with the Strings we made, or at least as much as we can
	for (int i = 0; i < bufferSize; i++) {
		if (i < tape->size + 1) {
			buffer[i] = upper[i];
		}
		else {
			buffer[i] = lower[i - tape->size - 1];
		}
	}
	
	// Clean up
	free(upper);
	free(lower);

}
