#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include"queue.h"
#include <stdbool.h>
#include "cmd.h"
#include <asm-generic/fcntl.h>
#include <sys/mman.h>

/* Expected call format: search <file_name> */
/* starts search from /home and finishes the search if a file with the searched <file_name> is found */

/* This file contains: */

/* getreelPath() */
/* shortens the constructed path where the file is found to a reel working path */

/* change_cd() */
/* calls the command cd which is implemented in cd.c via a child process */

#define BUFFER_SIZE 512


//does set the path to the reel path, because the file_name is allways appenden in the end
char *getReelPath(char *path) {
	int m = strlen(path);
	char *reelpath = path;
        int i = m;
        for (i = m - 1; path[i] != '/'; i--) {
	}
	for (int j = 0; j < i; j ++) {
		reelpath[j] = path[j];
	}
	reelpath[i] = '\0';
	//printf("shortened the path to: %s\n", reelpath);
	return reelpath;
}

//runs the cd command implemented with cd.c
void *change_cd(char *data) {
    char *cmd_exec = "./cd";
    char *flag = "~f";
    char *empty = "-";
    char *exe[] = {cmd_exec, flag, data, empty, NULL};
    pid_t pid = fork();
        int status;
        
    if (pid < 0) {
        printf("[fun execute]: fork error\n");
    } else if (pid == 0) {	// child
        execvp(exe[0], exe);
        switch(errno) {
            case 2:
                printf("Error[shell execute]: command %s not found\n", exe[0]);
                break;
            case 13:
                printf("Error[shell execute]: permission denied\n");
                break;
            default:
                printf("DEBUG: execute: unhandled errno %d\n", errno);
        }
        exit(errno);	// ends the forked child process and ensures that exit works	
    } else {	// parent
        waitpid(pid, &status, 0);
    }
}

int main(int arc, char **argv) {
    if (arc < 3) {
		printf("ERROR[search main]: wrong number of arguments\n");
		return -1;
	}
    char *cwd = "/home";
    //printf("cwd: %s\n", cwd);
    char *file_name = argv[2];
    //printf("file_name = %s\n", file_name);
    struct searchdata sdata[800];
    bool found = false;
    int i = 0;
    struct files files[800];
    struct node *start = make_node(cwd);
    enqueue(start);
    sdata[i].twd = cwd;
    int index = 0; 
    int count = 0;
    char *path_for_open_cd;
    char *found_path;
    //starts breadth first search
    while (!found) {
        //printf("size of queue: %d\n", size());
        count = 0;
        struct node *n = dequeue();
        char buffer[BUFFER_SIZE];
        DIR* dir;
        //printf("path: %s\n", n->dir);
		dir = opendir(n -> dir);
		struct dirent *d;
		struct stat s;

        //appends all directories into the queue and adds all files to the files struct.
        while ((d = readdir(dir)) != NULL) {
			sprintf(buffer, "%s/%s", n -> dir, d -> d_name);
			stat(buffer, &s);
            if (S_ISDIR(s.st_mode) && (d -> d_name)[0] != '.') {
                struct node *new = make_node(buffer);
                enqueue(new);
                i++;
                //printf("added dir: %s\n", buffer);
                sdata[i].twd = buffer;
            } else if (S_ISREG(s.st_mode)){
            	//printf("sdata[%d] before file added: %s\n", index, sdata[index].twd);
                files[count].name = d -> d_name;
                //printf("file: %s\n", d -> d_name);
                count++;
            }
        }
        closedir(dir);
        int j = 0;

        //check if file is found allready
        while (count != 0) {
            if (strcmp(files[j].name, file_name) == 0) {
                found = true;
                found_path = sdata[index].twd;
                path_for_open_cd = getReelPath(found_path);
                break;
            }
            count--;
            j++;
        }
        //printf("[sdata[%d] is: %s\n", index, sdata[index].twd);
        index ++;
        destroy_node(n);
    }
    if (found) {
        printf("Found the file at: %s, Filename: %s\n", path_for_open_cd, file_name);
        change_cd(path_for_open_cd);//sdata[index].twd
    } else {
        printf("File <%s> not found.\n", file_name);
    }
}
