#include <stdio.h> // fprintf()
#include <stdlib.h> // exit()
#include <unistd.h>

/*
receives input in the format
	command = "./mf" = argv[0]
	flag = "-" = argv[1]
	inpu1 = "<file_name>" = argv[2]
	input2 = "-" = argv[3]
*/
int main(int argc, char **argv) {
	// verify input
	if (argc != 4 || *argv[1] != '-' || *argv[2] == '-' || *argv[3] != '-') {
		printf("Syntax error - expected format: mf <file_name>\n"); // note: user input format differs from input format receivde from shell
		return -1;
	}
	FILE *fileptr;
	fileptr = fopen(argv[2], "wb");
	if (fileptr == NULL) {
		perror("mf: fopen() failed\n");	// printf() may be redirected
		exit(1);
	}
	fclose(fileptr);
	return 0;
	
}
