#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#define RIGHTS 0777 // 0777: give read, write, and execute rights to owner, group, and others

/* Expected call format md dir_name */
int md(const char *dir_name) {
	if (mkdir(dir_name, RIGHTS) == 0) {
		return 0;		
	} else {
		return errno;
	}
}

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("Syntax error - expected format: md dir_name");
		return -1;
	}
	int ret = md(argv[1]);
	if (ret != 0) {	// error handling
		switch (ret) {
			case 2:
				printf("(Parent) directory in %s not found\n", argv[1]);
				break;
			case 17:
				printf("Directory %s could not be created as it already exists\n", argv[1]);
				break;
			default:
				printf("Unspecified error [errno: %d]\n", ret);
		}			
		return -1;
	}		
	return 0;
}