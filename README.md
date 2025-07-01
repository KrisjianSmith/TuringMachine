# TuringMachine
This is an implementaiton of a Turing machine, written in C. A Turing Machine is a simple representation of a computer, consisting a set of states and
an infinite tape. The input for the machine is put on the infinite tape, and the input from the tape determines what action the Turing
machine will take. For a more detailed description of a turing machine, check out the [Wikipedia](https://en.wikipedia.org/wiki/Turing_machine) article
on Turing machines.

Specific instructions on using the project can be found in the `doc` directory of this repository.

Example turing machines can be found in the `files` directory of this repository.

Please note that I created this project in my own time between classes while studying for my degree, so the
documentation and testing is limited. I plan to return to this project one day to add more features and
more comprehensive documentation.

## How to build this project
First, ensure that the project is cloned to your local machine. The provided Makefile can be used to quickly
build the project. First, set up the project directory structure by typing `make clean`. Then, build the project
by typing `make`. This will build the binary, named `TuringMachine.`

## Command Line Usage

The program takes an input file, which represents the states of the turing machine. The syntax for input
files can be found under `doc/Syntax/Syntax.txt`. This file mustbe preceded with the `-I` switch, and must
be followed by the input to put on the tape. Additionally, there are four optional command line switches
that can be used:

- v - Verbose: Every step through the input is displayed
- s - Step: The program will pause after each step
- c - Clear: The output terminal will be cleared after every step
- o - Output: Rather than run the turing machine, the program will cross-compile my turing machine syntax
      into the syntax used by [This](https://turingmachinesimulator.com/) online Turing machine simulator.
      This was the turing machine simulator used by the professor for my automata class.


An example of the command line usage is as follows:

`./TuringMachine -vsc -I files/0n1n.txt 0011`

## Example Files

This program contains some example turing machine files under the `files` directory. Here is a brief description
of each file:

- 0n1n.txt - Accepts any string in the form 0^n 1^n on the input tape
- 0n1nVeryComplicated.txt - Accepts strings in the form 0^n 1^n by creating a universal turing machine on the current
  turing machine and simulating the universal turing machine.
- binaryAdd.txt - Adds two binary numbers separated by a + (ie. 0011+0101)
- binaryMultiply.txt - The same as above, but with multiplication. Numbers must be separated by an x (ie 10x10)
- efficientMultiply.txt - The same as above, but using a more efficient multiplication algorithm
