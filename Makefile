CC := gcc
FLAGS := -Wall -I src/

OBJECTS := bin/TuringMachine/TuringMachine.o \
		   bin/Main/main.o \
		   bin/TuringMachine/RuleTree.o \
		   bin/TuringMachine/Tape.o \
		   bin/Debug/Debug.o \
		   bin/Assembler/Assembler.o

TuringMachine: $(OBJECTS)
	$(CC) $^ $(FLAGS) -o $@

bin/Main/main.o: src/Main/main.c src/TuringMachine/TuringMachine.h
	$(CC) -c src/Main/main.c $(FLAGS) -o $@

bin/TuringMachine/TuringMachine.o: src/TuringMachine/TuringMachine.c \
								   src/TuringMachine/TuringMachine.h \
								   src/TuringMachine/Tape.c \
								   src/TuringMachine/Tape.h \
								   src/TuringMachine/RuleTree.h \
								   src/TuringMachine/RuleTree.h
	$(CC) -c src/TuringMachine/TuringMachine.c $(FLAGS) -o $@

bin/TuringMachine/RuleTree.o: src/TuringMachine/RuleTree.c src/TuringMachine/RuleTree.h
	$(CC) -c src/TuringMachine/RuleTree.c $(FLAGS) -o $@

bin/TuringMachine/Tape.o: src/TuringMachine/Tape.c src/TuringMachine/Tape.h
	$(CC) -c src/TuringMachine/Tape.c $(FLAGS) -o $@

bin/Debug/Debug.o: src/Debug/Debug.c src/Debug/Debug.h	
	$(CC) -c src/Debug/Debug.c $(FLAGS) -o $@

bin/Assembler/Assembler.o: src/TuringMachine/TuringMachine.h \
						   src/TuringMachine/RuleTree.h \
						   src/Assembler/Assembler.h \
						   src/Assembler/Assembler.c
	$(CC) -c src/Assembler/Assembler.c $(FLAGS) -o $@

clean:
	rm -rf bin
	mkdir bin
	mkdir bin/TuringMachine
	mkdir bin/Main
	mkdir bin/Debug
	mkdir bin/Assembler
	rm TuringMachine
