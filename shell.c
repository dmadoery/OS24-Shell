#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 
//#include<readline/readline.h> 
//#include<readline/history.h>

#define MAX_INPUT_LENGTH 40	/* maximal length of shell command */

//parser for "|"
void pipe_detector() {
    //TODO Implement
}

//parser for " "
void space_parser() {
    //TODO Implement
}

//executes piped and single commands via child processes with pipe communication if there are multiple cmds
void exe_cmd() {
    //TODO Implement
}

//switch case for all cmds
void cmd_finder() {
    //TODO Not sure if needed as own function
}

//main funciton of customized shell
int main () {
		char user_input[MAX_INPUT_LENGTH];
 		while(1) {
 				printf("$ ");
 				scanf("%s", user_input);
 				// call parser
 				printf("Input: %s\n", user_input);
 		}  
    return 0;
}
