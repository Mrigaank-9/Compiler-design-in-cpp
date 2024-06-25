# Compiler-design-in-cpp
Simple Lexer and Parser for a Subset of C++
This project implements a simple lexer and parser for a subset of the C++ language. It reads input code, tokenizes it using the lexer, and then parses the tokens to generate basic assembly-like instructions. The parser handles variable declarations, assignments, if statements, while loops, and print statements.

Features
Lexer: Tokenizes input strings into different types of tokens such as integers, floats, identifiers, operators, and keywords.
Parser: Parses the tokens to generate corresponding assembly-like instructions.
Supported Statements:
Variable Declarations (int and float)
Assignments
If Statements with optional else blocks
While Loops
Print Statements
Token Types
The lexer recognizes the following token types:

INT: Integer literals
FLOAT: Floating-point literals
IDENTIFIER: Variable names and keywords
Operators: +, -, *, /, =, >
Punctuation: ;, (, ), {, }
Keywords: if, else, while, print
END: End of input
ERROR: Unrecognized characters
