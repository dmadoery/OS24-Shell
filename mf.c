#include <fcntl.h> // O_RDWR
#include <stdio.h> // fprintf()
#include <stdlib.h> // exit()
#include <unistd.h>
#include <sys/mman.h>

#include "cmd.h"

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
	// open shared memory
	char *cwd;
	int shmfd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR);
	if (shmfd == -1) {
		printf("[pwd2] shm_open failed\n");
	}
	struct dfshm *data = mmap(NULL, sizeof(struct dfshm), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
	if (data == NULL) {
		printf("[pwd2] mmap failed\n");
	}
	cwd = data->current_working_dir;
	munmap (data, sizeof (struct dfshm));
    close (shmfd);
    
    int m = strlen(cwd);
    int n = strlen(argv[2]);
	
	FILE *fileptr;
	fileptr = fopen(argv[2], "wb");
	if (fileptr == NULL) {
		perror("mf: fopen() failed\n");	// printf() may be redirected
		exit(1);
	}
	fclose(fileptr);
	return 0;
	
}
