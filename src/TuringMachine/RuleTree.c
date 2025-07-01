#include "TuringMachine/RuleTree.h"
#include <stdlib.h>
#include <string.h>

// The default size of any variable-length array
const int DEFAULT_SIZE = 4;

// The max length of a state name
const int MAX_NAME_LEN = 200;

// Make a new RuleTree
RuleTree* makeRuleTree() {
	RuleTree* result = (RuleTree*) malloc(sizeof(RuleTree));

	// Make states array with the default size
	result->numStates = 0;
	result->stateCapacity = DEFAULT_SIZE;
	result->states = (State**) malloc(sizeof(State*) * DEFAULT_SIZE);
	return result;
}

// Frees all memory reserved by a state
void freeState(State* state) {
		
	// Free the rules array
	for (int i = 0; i < state->numRules; i++) {
		free(state->rules[i]);
	}
	free(state->rules);
	
	free(state->name);

	// Free the state itself
	free(state);
}

// Frees the memory reserved by a RuleTree
void freeRuleTree(RuleTree* tree) {
	
	// Free all states
	for (int i = 0; i < tree->numStates; i++) {
		freeState(tree->states[i]);
	}

	// Free the states array
	free(tree->states);

	// Free the tree itself
	free(tree);
}

// Makes a state with the given name. The rules array will be empty.
State* makeState(char* name, _Bool accept) {
	
	// Check that the name is not yet used 
	State* result = (State*) malloc(sizeof(State));

	// Set name
	result->name = (char*) malloc(sizeof(char) * MAX_NAME_LEN);
	strcpy(result->name, name);

	result->acceptState = accept;

	// Make new, empty rules array
	result->rules = (Rule**) malloc(sizeof(Rule*) * DEFAULT_SIZE);
	result->numRules = 0;
	result->ruleCapacity = DEFAULT_SIZE;

	return result;
}

// Double the size of the states array in the given tree
void growStatesArray(RuleTree* tree) {
	
	// Make a new States array with double the capacity
	State** newStates = (State**) malloc(sizeof(State*) * 2 * tree->stateCapacity);

	// Copy states into new array
	for (int i = 0; i < tree->numStates; i++) {
		newStates[i] = tree->states[i];
	}

	// Free old states array and replace it with newStates
	free(tree->states);
	tree->states = newStates;

	// Double the capacity
	tree->stateCapacity *= 2;
}

// Returns true if no state in the given tree has the given name
_Bool isUniqueName(RuleTree* tree, char* name) {
	for (int i = 0; i < tree->numStates; i++) {
		if (strcmp(name, tree->states[i]->name) == 0) {
			return 0;
		}
	}

	return 1;
}

// Adds a new state with the given name. If a state with the given name already exists,
// false is returned.
_Bool addNewState(RuleTree* tree,  char* name, _Bool accept) {

	if (strlen(name) > MAX_NAME_LEN || !isUniqueName(tree, name)) {
		return 0;
	}
	// Check if we have enough capacity
	if (tree->numStates == tree->stateCapacity) {
		growStatesArray(tree);
	}
	
	// Make the new state
	State* newState = makeState(name, accept);
	
	// Add new state to list and increment numStates
	tree->states[tree->numStates] = newState;
	tree->numStates++;

	return 1;
}


// Gets the index of the State with the given name.
// Returns -1 if no such state exists.
int getStateIdByName(RuleTree* tree, char* name) {
	for (int i = 0; i < tree->numStates; i++) {
		if (strcmp(name, tree->states[i]->name) == 0) {
			return i;
		}
	}
	return -1;
}

// Doubles the capacity of the rules array of the given state
void growRulesArray(State* state) {
	
	// Make a new Rules array with twice the capacity
	Rule** newRules = (Rule**) malloc(sizeof(Rule*) * state->ruleCapacity * 2);

	// Copy all rules into new array
	for (int i = 0; i < state->numRules; i++) {
		newRules[i] = state->rules[i];
	}

	// Free the old array and replace it with the new
	free(state->rules);
	state->rules = newRules;

	// Double the capacity
	state->ruleCapacity *= 2;
}

// Adds a new state for the state with the given id. The rule will have the given
// readsymbol, writeSymbol, moveDir, and stateToMoveTo
_Bool addNewRule(RuleTree* tree, char* stateName, char readSymbol, char writeSymbol, char moveDir, char* stateToMoveTo, char* errorBuffer, int errorBufferSize) {
	
	// Get the index of the state to move to and the state to move from
	int stateToAddToIndex = getStateIdByName(tree, stateName);
	int stateToMoveToIndex = getStateIdByName(tree, stateToMoveTo);
	
	// Check that the states exist
	if (stateToAddToIndex == -1) {
		char* part1 = "State ";
		char* part3 = " does not exist";
		
		// Check that the error buffer has sufficient size. If not, don't return any error message
		if (strlen(part1) + strlen(stateName) + strlen(part3) >= errorBufferSize - 1) {
			return 0;
		}

		strcpy(errorBuffer, part1);
		strcat(errorBuffer, stateName);
		strcat(errorBuffer, part3);
		return 0;
	}

	if (stateToMoveToIndex == -1) {
		char* part1 = "State ";
		char* part3 = " does not exist";
		
		// Check that the error buffer has sufficient size. If not, don't return any error message
		if (strlen(part1) + strlen(stateToMoveTo) + strlen(part3) >= errorBufferSize - 1) {
			return 0;
		}

		strcpy(errorBuffer, part1);
		strcat(errorBuffer, stateToMoveTo);
		strcat(errorBuffer, part3);
		return 0;
	}

	// Make sure that the moveDir is either '>' or '<'
	if (moveDir != '<' && moveDir != '>') {
		char part1[] = "Invalid move direction: X";
		part1[strlen(part1) - 1] = moveDir;
		if (strlen(part1) >= errorBufferSize - 1) {
			return 0;
		}
		strcpy(errorBuffer, part1);
		return 0;
	}

	// Get the state
	State* state = tree->states[stateToAddToIndex];

	// Check to see if the given state already has a rule for the current symbol
	for (int i = 0; i < state->numRules; i++) {
		if (state->rules[i]->readSymbol == readSymbol) {
			return 0;
		}
	}

	// Check that we have enough capacity. If not, grow the rule
	if (state->numRules == state->ruleCapacity) {
		growRulesArray(state);
	}

	// Make the rule
	Rule* rule = (Rule*) malloc(sizeof(Rule));
	rule->readSymbol = readSymbol;
	rule->writeSymbol = writeSymbol;
	rule->moveDir = moveDir;
	rule->stateToMoveTo = stateToMoveToIndex;

	// Add the rule to the list and increase the number of rules
	state->rules[state->numRules] = rule;
	state->numRules++;

	return 1;
}

// Returns the rule for the given RuleTree that corresponds to the state id and symbol provided
Rule* getRule(RuleTree* tree, int stateId, char symbol) {
	
	// Get the state
	State* state = tree->states[stateId];

	// Get the rule with the correct readSymbol
	for (int i = 0; i < state->numRules; i++) {
		if (state->rules[i]->readSymbol == symbol) {
			return state->rules[i];
		}
	}

	// If we didn't find any rule with that symbol, return null
	return NULL;
}

_Bool isAcceptState(RuleTree* tree, int stateId) {
	return tree->states[stateId]->acceptState;
}

void ruleTreeToString(RuleTree* tree, char* buffer, int bufferSize) {

	buffer[0] = '\0';
	
	// Iterate through all states
	for (int i = 0; i < tree->numStates; i++) {

		State* state = tree->states[i];

		// If we have room, add the state name to the buffer
		if (strlen(state->name) + strlen(buffer) + 1 >= bufferSize - 1) {
			return;
		}
		strcat(buffer, state->name);
		strcat(buffer, "\n");

		// Iterate through all rules in the state name
		for (int j = 0; j < state->numRules; j++) {

			Rule* rule = state->rules[j];

			// If we have room, add the rule to the buffer
			char firstPart[] = "\tX X X ";
			firstPart[1] = rule->readSymbol;
			firstPart[3] = rule->writeSymbol;
			firstPart[5] = rule->moveDir;

			if (strlen(tree->states[rule->stateToMoveTo]->name) + strlen(firstPart) + strlen(buffer) + 1 >= bufferSize - 1) {
				return;
			}
			strcat(buffer, firstPart);
			strcat(buffer, tree->states[rule->stateToMoveTo]->name);
			strcat(buffer, "\n");
		}
	}
}
