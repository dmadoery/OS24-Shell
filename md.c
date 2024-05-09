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

/*
receives input in the format
	command = ./md = argv[0]
	flag = "-" = argv[1]
	inpu1 = dir_name = argv[2]
	input2 = "-" = argv[3]
*/
int main(int argc, char **argv) {
	if (argc != 4 || *argv[1] != '-' || *argv[2] == '-' || *argv[3] != '-') {
		printf("Syntax error - expected format: md dir_name\n"); // note: user input format differs from input format receivde from shell
		return -1;
	}
	int ret = md(argv[2]);
	if (ret != 0) {	// error handling
		switch (ret) {
			case 2:
				printf("(Parent) directory in %s not found\n", argv[2]);
				break;
			case 17:
				printf("Directory %s could not be created as it already exists\n", argv[2]);
				break;
			default:
				printf("Unspecified error [errno: %d]\n", ret);
		}			
		return -1;
	}		
	return 0;
}
