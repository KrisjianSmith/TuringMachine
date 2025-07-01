#include "Assembler/Assembler.h"
#include <stdio.h>
#include <stdlib.h>

void assembleTuringMachine(char* inputFile, char* outputFile) {
	
	printf("Assembler functionality not yet implemented, sorry!\n");
	return;


	// Set up files
	FILE* input = fopen(inputFile, "r");
	FILE* output = fopen(outputFile, "w");

	if (input == NULL) {
		printf("Error opening file: %s\n", inputFile);
		return;
	}

	if (output == NULL) {
		printf("Error opening file: %s\n", inputFile);
	}

	TuringMachine* tm = makeTuringMachine();
	
	fclose(input);
	fclose(output);
	freeTuringMachine(tm);
}
