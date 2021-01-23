# XMicro-Compiler
Scanner and a Top Down Parser Compiler For A Small Scale Language Called XMicro

## Program Options
### Option 1
* The program asks the user for a source code file to scan and an output file to write the sequence of tokens to
* If there are lexical errors then errors are printed to the screen

### Option 2
* The program asks the user for the source code file to parse the source code
* If there are no lexical errors and no syntax errors, a message indicating that parsing is successful is printed to the screen.
* If there are lexical errors and syntax errors, error messages are printed to the screen.

## EBNF Rules
* The XMicro language has the following lexical and syntax rules
* EBNF grammar rules describe the syntax of the XMicro language
* THe Parser will be based on these grammar rules

## Token Types
* Determines all possible token types in the XMicro language
* Must scan and extract each token type
* The scanner will be based on these token types
