#include<stdio.h>
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 
#include"cmd.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <assert.h>
//#include<readline/readline.h> 
//#include<readline/history.h>

#define MAX_INPUT_LENGTH 40	/* maximal length of shell command */

#define DEBUG_MAX_IT 10

// shared memory
int shmfd;
struct dfshm *data;

char ** split(char *in_str, const char *delim);
int count(char *in_str, const char *delim);

/* called by the parser to execute the commands
cmds: array with the commands to be executed
length: number of commands in cmds */
void execute(struct cmd *cmds, int length) {
	for(int i = 0; i < length; i++) {
		//printf("Command: %s\n", cmds[i].command);
		//printf("Flag: %s\n", cmds[i].flag);
		//printf("Input 1: %s\n", cmds[i].input1);
		//printf("Input 2: %s\n", cmds[i].input2);
	}
	if (length == 1) {
		// no piping
		char *cmd_str = cmds[0].command;
		int n = strlen(cmd_str);
		// turn "cmd_str" to "./cmd_str\0" (e.g., turn md into "./md\0")
		char cmd_exec[n + 3];
		cmd_exec[0] = '.';
		cmd_exec[1] = '/';
		for (int i = 0; i < n; i++) {
			cmd_exec[i + 2] = cmd_str[i];
		}
		cmd_exec[n + 2] = '\0'; // Maybe adding /0 should be done sooner?
		char *argv[] = {cmd_exec, cmds[0].flag, cmds[0].input1, cmds[0].input2, NULL};
		pid_t pid = fork();
		int status;
		
		if (pid < 0) {
			printf("[fun execute]: fork error\n");
		} else if (pid == 0) {	// child
			execvp(argv[0], argv);
			switch(errno) {
				case 2:
					printf("Error in execute: command %s not found\n", argv[0]);
					break;
				default:
					printf("DEBUG: execute: unhandled errno %d\n", errno);
			}		
		} else {	// parent
			waitpid(pid, &status, 0);
		}		
	} else if (length > 1) {
		// TODO piping
	} else { // length < 1
		printf("[fun execute]: length < 1\n");
	}
}

void parser(char *in_str) {
    int n = count(in_str, "|") + 1;
	printf("Number of commands: %d\n", n);
	struct cmd cmds[n];
	for (int i = 0; i < n; i++) {
		cmds[i].command = "-"; //needs to be initalized with "-" to check if empty
		cmds[i].flag = "-";
		cmds[i].input1 = "-";
		cmds[i].input2 = "-";
	}
	
	char **full_cmd = split(in_str, "|");
	for(int j = 0; j < n; j++) {
		char **splited_cmd = split(full_cmd[j], " ");
		int m = count(full_cmd[j], " ") + 1;
		for (int i = 0; i < m; i++) {
			printf("%s\n", splited_cmd[i]);
			printf("%ld\n", strlen(splited_cmd[i]));
			if (i == 0) {
				cmds[j].command = splited_cmd[i];
				//printf("i=0\n");
			}
			if (i == 1 && *splited_cmd[i]  ==  '~') { //126 is '~'
				cmds[j].flag = splited_cmd[i];
				//printf("i=1/if, :%c\n", *splited_cmd[i]);
			} else if (i == 1) {
				cmds[j].input1 = splited_cmd[i];
				//printf("i=1/else\n");
			}
			if (i == 2 && *cmds[j].input1 == '-') { //45 is '-' and is the empty symbol for this shell
				cmds[j].input1 = splited_cmd[i];
				//printf("i=2/if\n");
			} else if (i == 2) {
				cmds[j].input2 = splited_cmd[i];
				//printf("i=2/else\n");
			}
			if (i == 3 && *cmds[j].input2 == '-') {
				cmds[j].input2 = splited_cmd[i];
				//printf("i=3\n");
			}
		}
	}

	for(int i = 0; i < n; i++) {
		printf("Command: %s\n", cmds[i].command);
		printf("Flag: %s\n", cmds[i].flag);
		printf("Input 1: %s\n", cmds[i].input1);
		printf("Input 2: %s\n", cmds[i].input2);
	}
	execute(cmds, n);
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
	//printf("[copy] length: %d\n", length);
	char *out_str;
	out_str = (char *) malloc(length * sizeof(char));
	for (int i = 0; i < length; i++) {
		//printf("i: %d\n", i);
		out_str[i] = in_str[i];
	}
	//printf("[copy] out_str: %s\n", out_str);
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
	
	//printf("nr_of_delims: %d\n", nr_of_delims);
	//printf("nr_of_chars: %d\n", nr_of_chars);
	char sub_str[nr_of_chars - nr_of_delims];
	char **out_str;
	out_str = (char **) malloc((nr_of_delims + 1) * sizeof(char *));
	
	int i = 0;
	int j = 0;
	int offset = 0;
	
	for (j = 0; j < nr_of_chars; j++) {
		//printf("j: %d\n", j);
		if (in_str[j] == *delim) {
			out_str[i] = copy(sub_str, j - offset);
			//printf("here\n");
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

	//input output 
	//pipe or not pipe
	//if else for cmds checker
	//execute cmd in pipe or seperate
}

//switch case for all cmds
void cmd_checker() {
    //TODO Not sure if needed as own function
}

//initialize user/pc name/ path
void init() { 
	char *user_name;
	//char pc_name[MAX_INPUT_LENGTH];
	//user_name = getlogin();
	user_name = getenv("USER");
	//gethostname(pc_name, sizeof(pc_name));
	int n = strlen(user_name);
	printf("n = %d\n", n);
	char dir_path[n+7];
	dir_path[0] = '/';
	dir_path[1] = 'h';
	dir_path[2] = 'o';
	dir_path[3] = 'm';
	dir_path[4] = 'e';
	dir_path[5] = '/';
	for (int i = 0; i < n; i++) {
		dir_path[i + 6] = user_name[i];
	}
	dir_path[n+6] = '\0';
	printf("[init] %s\n", dir_path);
	
	//share memory
	shmfd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	assert( shmfd != 1);
	assert(ftruncate(shmfd, sizeof(struct dfshm)) != -1);
	data = mmap(NULL, sizeof(struct dfshm), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
	assert(data != MAP_FAILED);
	//data->current_working_dir = dir_path;
	strcpy(data->current_working_dir, dir_path);
	//data->pc_name = pc_name;
	printf("[init] %s\n", data->current_working_dir);  
}

/* Called before exiting the shell to properly free memory and close shared memory */
void finally() {
	munmap (data, sizeof (struct dfshm));
    close (shmfd);
    shm_unlink (SHM_NAME);
}

//main funciton of customized shell
int main () {
	printf("Welcome to Shell!!!\n");
	printf("Type exit to exit shell\n");
	init();
    char user_input[MAX_INPUT_LENGTH];
    //while(1) { debugging
    for (int i = 0; i < DEBUG_MAX_IT; i++) {
        printf("$ ");
        //scanf("%[^\n]%*c", user_input);
        fgets(user_input, MAX_INPUT_LENGTH, stdin);
        user_input[strcspn(user_input, "\n")] = 0;
        // call parser
        if (strlen(user_input) == 1) {
        	printf("DEBUG: zero-length user input\n");
        	continue;
        }
        if (strcmp(user_input, "exit") == 0) {
        	finally();
        	break;
        }
        printf("Input: %s\n", user_input);
        //char ** o;
        parser(user_input);
        //int n = count(user_input, "|");
        //for (int i = 0; i <= n; i++) {
        //	printf("sub: %s\n", o[i]);
        //}
        
    } 
    finally(); 
    return 0;
}

//test input: test1 ~c hi you|test2 ~t a b
