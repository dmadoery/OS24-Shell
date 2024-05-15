#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>	// chdir

/* Expected call format cd dir_name */
int cd(const char *dir_name) {
	if (chdir(dir_name) == 0) {
		return 0;		
	} else {
		return errno;
	}
}

/*
receives input in the format
	command = ./cd = argv[0]
	flag = "-" = argv[1]
	inpu1 = dir_name = argv[2]
	input2 = "-" = argv[3]
*/
int main(int argc, char **argv) {
	if (argc != 4 || *argv[1] != '-' || *argv[2] == '-' || *argv[3] != '-') {
		printf("Syntax error - expected format: cd dir_name\n"); // note: user input format differs from input format receivde from shell
		return -1;
	}
	char dir[128];
	//mkdir("Test", 0777);
	//cd("Test");
	printf("%s\n", getcwd(dir, 128));
	int ret = cd(argv[2]);
	if (ret != 0) {	// error handling
		switch (ret) {
			case 2:
				printf("No directory %s\n", argv[2]);
				break;
			default:
				//printf("%s\n", strerror(ret));
				printf("Unspecified error [errno: %d]\n", ret);
		}			
		return -1;
	}
	printf("%s\n", getcwd(dir, 128));

	return 0;
}
