#include <stdio.h>
#include <stdlib.h>

/*
receives input in the format
	command = "./cp" = argv[0]
	flag = "-" = argv[1]
	inpu1 = "<from_file>" = argv[2]
	input2 = "<to_file>" = argv[3]
*/
int main(int argc, char **argv) {
	if (argc != 4 || *argv[1] != '-' || *argv[2] == '-' || *argv[3] == '-') {
		printf("Syntax error - expected format: cp <from_file> <to_file>\n"); // note: user input format differs from input format receivde from shell
		return -1;
	}
	FILE *from_file;
	FILE *to_file;
	from_file = fopen(argv[2], "r");
	if (from_file == NULL) {
		perror("mf: fopen() failed for from_file\n");	// printf() may be redirected
		exit(1);
	}
	to_file = fopen(argv[3], "w");
	if (to_file == NULL) {
		perror("mf: fopen() failed for to_file\n");	// printf() may be redirected
		exit(1);
	}
	char line[128];
	while (fgets(line, sizeof(line), from_file)) {
		fprintf(to_file, "%s", line);
	}	
	fclose(from_file);
	fclose(to_file);
}
