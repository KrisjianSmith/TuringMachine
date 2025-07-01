#ifndef TURING_MACHINE
#define TURING_MACHINE

#include "TuringMachine/Tape.h"
#include "TuringMachine/RuleTree.h"

// The Turing Machine
typedef struct {
	
	// The current state of the TM. The start state should be 0.
	int state;

	// The tree of rules that defines the states and state transitions of
	// the Turing Machine
	RuleTree* rules;

	// The tape of the Turing Machine
	Tape* tape;

	// The number of steps that has been taken
	int steps;

} TuringMachine;

// Makes a turing machine
TuringMachine* makeTuringMachine();

// Frees a turing machine and all associated memory
void freeTuringMachine(TuringMachine* tm);

// Loads a Turing Machine from a file
_Bool loadTuringMachineFromFile(TuringMachine* tm, char* fileName, char* errorMessage, int errorMessageSize);

// Starts the Turing Machine with the given input on the Tape
void executeInput(TuringMachine* tm, char* input, _Bool verbose, _Bool step, _Bool clear);

#endif
