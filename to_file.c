#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 128

/*
receives input in the format
	command = "./to_file" = argv[0]
	flag = "-" = argv[1]
	inpu1 = "<to_file>" = argv[2]
	input2 = "-" = argv[3]
*/
int main(int argc, char **argv) {
	if (argc != 4 || *argv[1] != '-' || *argv[2] == '-' || *argv[3] != '-') {
		perror("Syntax error - expected format: to_file <[to_path/]to_file>\n"); // note: user input format differs from input format receivde from shell
		exit(1);
	}
	FILE *to_file;
	to_file = fopen(argv[2], "w");
	if (to_file == NULL) {
		perror("ERROR[to_file main]: fopen() failed for to_file\n");	// printf() may be redirected
		exit(1);
	}
	char buffer[MAX_LINE];
	while(fgets(buffer, sizeof(buffer), stdin)) {
		fprintf(to_file, "%s", buffer);
	}
	fclose(to_file);
}
