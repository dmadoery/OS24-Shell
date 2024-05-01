#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PATH_MAX 1024	// used in pwd()

struct cmd {
    char *command;
    char *flag;
    char *input1;
    char *input2;
};

/* makes a new directory name dir_name under the current working directory */
int md(const char *dir_name) {
	if (mkdir(dir_name, 0777) == 0) {;	// 0777: give read, write, and execute rights to owner, group, and others
		return 0;
	} else {
		return errno;
	}
}

/* displays the current working directory */
int pwd() {
	char cwd[PATH_MAX];	// cwd = current working directory
	if (getcwd(cwd, sizeof(cwd)) != NULL) {	// copies an absolute pathname of the current working directory to cwd
		printf("%s\n", cwd);
	} else {
		return errno;
	}
}
