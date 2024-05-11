#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PATH_MAX 1024

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
