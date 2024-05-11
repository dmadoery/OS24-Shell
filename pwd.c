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
		printf("$Current directory: %s\n",cwd);
	} else {
		return errno;
	}
}

int main(int argc, char **argv) {
    pwd();
	return 0;
}
