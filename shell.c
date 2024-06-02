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

// log file
FILE *file;

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
			printf("ERROR[shell execute]: fork error\n");
		} else if (pid == 0) {	// child
			execvp(argv[0], argv);
			switch(errno) {
				case 2:
					printf("ERROR[shell execute]: command %s not found\n", argv[0]);
					break;
				case 13:
					printf("ERROR[shell execute]: permission denied\n");
					break;
				default:
					printf("DEBUG[shell execute]: unhandled errno %d\n", errno);
			}
			_exit(errno);	// ends the forked child process and ensures that exit works. Note _exit() recommended instead of exit()
		} else {	// parent
			waitpid(pid, &status, 0);
		}
	} else if (length == 2) {
		// Parent saves stdin (0) and stdout (1)
		int tempin = dup(0);
		int tempout = dup(1);
		
		int fdpipe[2];
		pipe(fdpipe); // pipe must be created in parent such that both children have access
		
		dup2(fdpipe[1], 1);	// stdout now refers to fdpipe[1], the write-to end of the (uni-)directional pipe
		close(fdpipe[1]);	// since stdout is now a copy of fdpipe[1], the original is no longer needed
		
		// fork
		pid_t pid = fork();
		if (pid < 0) {
			perror("ERROR[shell execute]: fork error\n");
		} else if (pid == 0) {	// child	
			close(fdpipe[0]); // the read-from end of the pipe is not needed in the first process (only the second)
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
			execvp(argv[0], argv);
			// this section is only accessed if execvp fail
			switch(errno) {
				case 2:
					perror("ERROR[shell execute]: command not found\n");
					break;
				case 13:
					perror("ERROR[shell execute]: permission denied\n");
					break;
				default:
					perror("DEBUG[shell execute]: unhandled errno\n");
			}
			_exit(errno);
		} else {	// parent
			dup2(fdpipe[0], 0);	// stdin now refers to fdpipe[0], the read-from end of the (uni-)directional pipe
			close(fdpipe[0]);	// close the original
			
			dup2(tempout, 1); // restore stdout (1)
			// fork
			pid_t pid = fork();
			if (pid < 0) {
				perror("ERROR[shell execute]: fork error\n");
			} else if (pid == 0) {	// child			
				char *cmd_str = cmds[1].command;
				int n = strlen(cmd_str);
				// turn "cmd_str" to "./cmd_str\0" (e.g., turn md into "./md\0")
				char cmd_exec[n + 3];
				cmd_exec[0] = '.';
				cmd_exec[1] = '/';
				for (int i = 0; i < n; i++) {
					cmd_exec[i + 2] = cmd_str[i];
				}
				cmd_exec[n + 2] = '\0'; // Maybe adding /0 should be done sooner?
				char *argv[] = {cmd_exec, cmds[1].flag, cmds[1].input1, cmds[1].input2, NULL};
				execvp(argv[0], argv);
				// this section is only accessed if execvp fail
				switch(errno) {
					case 2:
						perror("ERROR[shell execute]: command not found\n");
						break;
					case 13:
						perror("ERROR[shell execute]: permission denied\n");
						break;
					default:
						perror("DEBUG[shell execute]: unhandled errno\n");
				}
				_exit(errno);
			} else {	// parent
				dup2(tempin, 0);	// restore stdin (0)
						
				waitpid(pid, NULL, 0);
			}
		}
		
	} else if (length > 2) {
		// TODO piping
	} else { // length < 1
		printf("DEBUG[shell execute]: length < 1\n");
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

int toggle_recording(int recording) {
	switch(recording) {
		case 0:	// turn on recording
			file = fopen("log.txt", "a");
			fprintf(file, "--- start recoring ---\n");
			break;
		case 1:	// turn off recording
			fprintf(file, "--- stop recoding ---\n");
			fclose(file);
			break;
	}
	return (++recording) % 2;
}

//main funciton of customized shell
int main () {
	printf("Welcome to Shell!!!\n"); 	// maybe have an help function for this ...
	printf("Type exit to exit shell\n");
	init();
	int recording = 0;	// 0 = False
    char user_input[MAX_INPUT_LENGTH];
    //while(1) { debugging
    for (int i = 0; i < DEBUG_MAX_IT; i++) {
        printf("$ ");
        //s
        fgets(user_input, MAX_INPUT_LENGTH, stdin);	// don't use canf("%[^\n]%*c", user_input); as an empty input results in infinite loop and hard crash of terminal
        user_input[strcspn(user_input, "\n")] = 0;	// removes the newline character '\n'
        // call parser
        if (recording && file != NULL) {
        	fprintf(file, "%s\n", user_input);
        }
        if (strlen(user_input) == 0) {
        	//printf("DEBUG: zero-length user input\n");
        	continue;
        }
        if (strcmp(user_input, "exit") == 0) {
        	//finally(); // can be moved ouside the while loop
        	break;
        } else if (strcmp(user_input, "record") == 0) {
        	recording = toggle_recording(recording);
        } else {
        	printf("Input: %s\n", user_input);
        	parser(user_input);
        }
    } 
    finally();
    if (recording) {
    	toggle_recording(recording);
    }
    return 0;
}

//test input: test1 ~c hi you|test2 ~t a b
