#ifndef TAPE
#define TAPE

extern const char BLANK;

// A node in the doubly-linked list in the tape
typedef struct TapeNode {
	struct TapeNode* next;
	struct TapeNode* prev;

	// The current character in the tape
	char symbol;
} TapeNode;

// A wrapping struct to store the current tape
typedef struct {
	int size;
	TapeNode* current;
} Tape;

// Make new tape
Tape* makeTape(char* input);

// Free memory used by tape
void freeTape(Tape* tape);

// Get the next character on the tape
char next(Tape* tape);

// Get the previous character on the tape
char previous(Tape* tape);

// Get the current character on the tape
char get(Tape* tape);

// Write the given character to the tape at the current position
void write(Tape* tape, char c);

// Turn tape into String of characters
void tapeToString(Tape* tape, char* buffer, int bufferSize);

#endif
