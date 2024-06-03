#include <fcntl.h> // O_RDWR
#include <stdio.h> // fprintf()
#include <stdlib.h> // exit()
#include <unistd.h>
#include <sys/mman.h>

#include "cmd.h"

/*
receives input in the format
	command = "./view" = argv[0]
	flag = "-" = argv[1]
	inpu1 = "<file_name>" = argv[2]
	input2 = "-" = argv[3]
*/
int main(int argc, char **argv) {
	// verify input
	if (argc != 4 || *argv[1] != '-' || *argv[2] == '-' || *argv[3] != '-') {
		printf("Syntax error - expected format: view <file_name>\n"); // note: user input format differs from input format receivde from shell
		return -1;
	}
	// open shared memory
	char *cwd;
	int shmfd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR);
	if (shmfd == -1) {
		printf("ERROR[view main]: shm_open failed\n");
		return -1;
	}
	struct dfshm *data = mmap(NULL, sizeof(struct dfshm), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
	if (data == NULL) {
		printf("ERROR[view main]: mmap failed\n");
		return -1;
	}
	cwd = data->current_working_dir;
    
    int m = strlen(cwd); // strlen() does not include '\0'
    int n = strlen(argv[2]);
	
	char path_file[m + n + 2];
	for (int i = 0; i < m; i++) {
		path_file[i] = cwd[i];
	}
	path_file[m] = '/';
	for (int i = 0; i < n; i++) {
		path_file[m + 1 + i] = argv[2][i];
	}
	path_file[m + n + 1] = '\0';

	FILE *fileptr;
	fileptr = fopen(path_file, "r");
	if (fileptr == NULL) {
		perror("ERROR[view main]: fopen() failed\n");	// printf() may be redirected
		return -1;
	}
	
	char *line = NULL;	// getline() will allocate a buffer for storing the line. This buffer should be freed.
	size_t len = 0;
	while (getline(&line, &len, fileptr) != -1) {
		printf("%s", line);
	}
	
	munmap (data, sizeof (struct dfshm));
    close (shmfd);
	fclose(fileptr);
	if (line) {
		free(line);
	}
	return 0;
	
}
