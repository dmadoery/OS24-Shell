#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#define PATH_MAX 1024
#define RIGHTS 0777 // 0777: give read, write, and execute rights to owner, group, and others

/* Expected call format md dir_name */
int pwd() {
	char cwd[PATH_MAX];	// cwd = current working directory
	if (getcwd(cwd, sizeof(cwd)) != NULL) {	// copies an absolute pathname of the current working directory to cwd
		printf("%s\n", cwd);
	} else {
		return errno;
	}
}

/*
receives input in the format
	command = ./md = argv[0]
	flag = "-" = argv[1]
	inpu1 = dir_name = argv[2]
	input2 = "-" = argv[3]
*/
int main(int argc, char **argv) {
    printf("testing");
	if (argv[0] != "pwd") {
		printf("Syntax error - expected format: pwd\n");
		return -1;
	}
	pwd();
	return 0;
}
