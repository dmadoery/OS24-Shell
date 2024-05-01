#include<stdio.h>
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 
#include"cmd.h"
//#include<readline/readline.h> 
//#include<readline/history.h>

#define MAX_INPUT_LENGTH 40	/* maximal length of shell command */
char ** split(char *in_str, const char *delim);
//
void parser(char *in_str) {
    int n = count(in_str, "|") + 1;
	struct cmd cmds[n];
	char *full_cmd = split(in_str, "|");
	for(int j = 0; j < n; j++) {
		char *splited_cmd = split(&full_cmd[j], " ");
		for (int i = 0; i < 4; i++) {
			if (i == 1) {
				cmds[j].command = &splited_cmd[i];
			}
			if (i == 2) {
				char *flg = &splited_cmd[i];
				if (&flg[0] == "~") {
					cmds[j].flag = flg;
				}
			}
			if (i == 3) {
				cmds[j].input1 = &splited_cmd[i];
			}
			if (i == 4) {
				cmds[j].input2 = &splited_cmd[i];
			}
		}
	}
	
}

/* Counts the number of occurences of the delimiter delim in the input strint in_str */
int count(char *in_str, const char *delim) {
	int counter = 0;
	char *tmp = in_str;
	
	while (*tmp) {
		if (*tmp == *delim) {
			counter++;
		}
		tmp++;
	}
	return counter;
}

/* Helper function for split */
char* copy(char* in_str, int length) {
	printf("[copy] length: %d\n", length);
	char *out_str;
	out_str = (char *) malloc(length * sizeof(char));
	for (int i = 0; i < length; i++) {
		printf("i: %d\n", i);
		out_str[i] = in_str[i];
	}
	printf("[copy] out_str: %s\n", out_str);
	return out_str;
}

/* Splits the input string in_str int an array of substrings at each occurence of the delimiter delim*/
char ** split(char *in_str, const char *delim) {
	int nr_of_delims = 0;
	int nr_of_chars = 0;
	char *tmp = in_str;
	
	while (*tmp) {
		if (*tmp == *delim) {
			nr_of_delims++;
		}
		nr_of_chars++;
		tmp++;
	}
	
	printf("nr_of_delims: %d\n", nr_of_delims);
	printf("nr_of_chars: %d\n", nr_of_chars);
	char sub_str[nr_of_chars - nr_of_delims];
	char **out_str;
	out_str = (char **) malloc((nr_of_delims + 1) * sizeof(char *));
	
	int i = 0;
	int j = 0;
	int offset = 0;
	
	for (j = 0; j < nr_of_chars; j++) {
		printf("j: %d\n", j);
		if (in_str[j] == *delim) {
			out_str[i] = copy(sub_str, j - offset);
			printf("here\n");
			//printf("%s\n", *out_str[i]);
			i++;
			offset = j + 1;
		} else {
			sub_str[j - offset] = in_str[j];
		}
	}
	out_str[i] = copy(sub_str, j - offset);
	
	return out_str;
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
        scanf("%[^\n]%*c", user_input);
        // call parser
        printf("Input: %s\n", user_input);
        //char ** o;
        parser(user_input);
        //int n = count(user_input, "|");
        //for (int i = 0; i <= n; i++) {
        //	printf("sub: %s\n", o[i]);
        //}
        
    }  
    return 0;
}
