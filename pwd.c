#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "cmd.h" // PATH_MAX

/* Expected call format pwd */
int pwd() {
	char cwd[PATH_MAX];	// cwd = current working directory
	if (getcwd(cwd, sizeof(cwd)) != NULL) {	// copies an absolute pathname of the current working directory to cwd
		printf("$Current directory: %s\n",cwd);
        return 0;
	} else {
		return errno;
	}
}

int main(int argc, char **argv) {
    pwd();
	return 0;
}
