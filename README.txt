COP4610 - Project 1


Project Team Members
- Holly Culver
- Abby Perry
- Hunter Davis

Division of Labor
- Holly Culver: Background Processing, Function declarations, code testing, pipe testing, makefile declaration
- Abby Perry: Piping, shell, process testing, code testing
- Hunter Davis: Parsing, background processing, code testing

How to Compile:
- Using the Makefile, type 'make'. The executable will be named MyShell.x
- Type MyShell.x

How to clean: 
-make clean 

Tar File Contents:
README.txt
makefile
MyFunctions.c
MyFunctions.h
MyParse.c
MyParse.h
MyProcesses.c
MyProcesses.h
MyShell.c
MyShell.h
main.c


Completed using: linprog


Brief Description of Each File:

-MyParse: grabs input from the user and extracts the command name, the arguments, and background execution indicators. Allocates memory for the parsing arguments and converts them to dynamic character arrays. Passes information from one process to the next, allows for communicaiton between multiple function calls. Manages and calculates the number of commands separated by "|"

MyProcesses: handles, creates, and checks the created processes to be manipulated for the completion of the shell 

-MyFunctions: Allocates and deallocates memory, checks in files exist, makes/deletes directories, creates/ changes/ builds paths, and prints arguments to screen.  Creates a background to test commands given from the user as input. Completes functions such as I/O redirection and executes the pipe function and does error checking for both of these. 

-MyShell: single run shell command that initiates the running of the shell process 

Known Bugs:


Unfinished Portions:
