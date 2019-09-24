# Shell-Project
Team members: Diego Gutierrez, Samuel Parmer and Christian Gazmuri

Division of labor:

Samuel Parmer: Prompt, Execution, background Processing, IO redirection, pipes, shortcut resolution, $Path Resolution, and built-ins.

Diego Gutierrez: ReadMe, makefile, IO redirection, pipes and built-ins(alias and unalias), error checking.

Christian:  Shortcut Resolution and built-ins(echo).

Tar archive info: The tar contains the main.c file and the makefile (and this README. The main.c is the c programs that runs the compiles to shell. The makefile compiles the said code. The README file specifies division of labor, how to compile, etc.

Makefile- type make to compile the shell. The shell is compiled as such by the makefile:

all:
	gcc main.c -o shell
  
Type "shell" after compilation to run the program.

Bugs, Unfinished sections, notes: Error checking fails when & cd & is called.

Pipes are partially implemetned, you can find our attempt at piping in the code. 

Background processes work but shell doesn't print the completion message for the first process. Proceeding processes print queue postion and pid upon completion. 

alias commands need spaces around the '=' to work. Error checking for alias is minimal.
