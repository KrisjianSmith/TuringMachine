#ifndef RULE
#define RULE

// The max length of a state name
extern const int MAX_NAME_LEN;

// The rule object that is stored in the RuleTree
typedef struct {

	// The symbol that needs to be on the tape for this rule to apply
	char readSymbol;	

	// The symbol to write on the tape
	char writeSymbol;

	// The direction to move in
	// '<' = left, '>' = right
	// char instead of boolean because I might want to add a
	// no movement option one day
	char moveDir;

	// The state to move to after writing the symbol
	int stateToMoveTo;

} Rule;

// A structure that represents a state in the TM
typedef struct {
	
	// Whether or not this state is an accept state
	_Bool acceptState;

	// The number of rules associated with this state
	int numRules;
	
	// The size of the rules array
	int ruleCapacity;

	// The name of the state (optional)
	char* name;

	// The list of rules for this TM. 
	Rule** rules;

} State;

// The structure that holds all the rules of the turing machine
typedef struct {
	
	// The number of states in the RuleTree
	int numStates;
	
	// The size of the states array
	int stateCapacity;

	// The list of states
	State** states;

} RuleTree;

RuleTree* makeRuleTree();
void freeRuleTree(RuleTree* tree);
_Bool addNewState(RuleTree* tree,  char* name, _Bool accept);
_Bool addNewRule(RuleTree* tree, char* stateName, char readSymbol, char writeSymbol, char moveDir, char* stateToMoveTo, char* errorBuffer, int errorBufferSize);
Rule* getRule(RuleTree* tree, int stateId, char symbol);
_Bool isAcceptState(RuleTree* tree, int stateId);
void ruleTreeToString(RuleTree* tree, char* buffer, int bufferSize);
#endif
