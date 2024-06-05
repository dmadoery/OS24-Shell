#include <fcntl.h> // O_RDWR
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "cmd.h" // PATH_MAX

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
	
	int m;
	char *cwd;
	if (argv[3][0] == '/') {	// if the to_path begins with '/', then it is an absolute path
		m = 0;
	} else {
		// open shared memory
		int shmfd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR);
		if (shmfd == -1) {
			perror("ERROR[cp pwd2]: shm_open failed\n");
			exit(1);
		}
		struct dfshm *data = mmap(NULL, sizeof(struct dfshm), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
		if (data == NULL) {
			perror("ERROR[cp pwd2]: mmap failed\n");
			exit(1);
		}
		pthread_mutex_lock(&shm_mutex_lock);
		cwd = data->current_working_dir;
		pthread_mutex_unlock(&shm_mutex_lock);
		
		m = strlen(cwd);
		
		munmap (data, sizeof (struct dfshm));
    	close (shmfd);
	}
	
	int n = strlen(argv[2]);
	char to_path_file[m + n + 2];
	
	for (int i = 0; i < m; i++) {
		to_path_file[i] = cwd[i];
	}
	to_path_file[m] = '/';
	for (int i = 0; i < n; i++) {
		to_path_file[m + 1 + i] = argv[2][i];
	}
	
	FILE *to_file;
	to_file = fopen(to_path_file, "w");
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
