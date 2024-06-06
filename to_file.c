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
	if (argc != 4 || (*argv[1] != '-' && strcmp(argv[1], "~a") != 0) || *argv[2] == '-' || *argv[3] != '-') {
		perror("Syntax error - expected format: to_file <[to_path/]to_file>"); // note: user input format differs from input format receivde from shell
		exit(1);
	}
	int m;
	char *cwd;
	int shmfd;
	struct dfshm *data;
	if (argv[2][0] == '/') {	// if the to_path begins with '/', then it is an absolute path
		m = 0;
	} else {
		// open shared memory
		shmfd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR);
		if (shmfd == -1) {
			perror("ERROR[cp pwd2]: shm_open failed");
			exit(1);
		}
		data = mmap(NULL, sizeof(struct dfshm), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
		if (data == NULL) {
			perror("ERROR[cp pwd2]: mmap failed");
			exit(1);
		}
		pthread_mutex_lock(&shm_mutex_lock);
		cwd = data->current_working_dir;
		pthread_mutex_unlock(&shm_mutex_lock);
		
		m = strlen(cwd);
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
	to_path_file[m + n + 1] = '\0';
	
	FILE *to_file;
	if (strcmp(argv[1], "~a") == 0) {
		to_file = fopen(to_path_file, "a");	// open for append
	} else {
		to_file = fopen(to_path_file, "w");	// open for writing. If the file exists, its contents are overwritten.
	}
	if (to_file == NULL) {
		perror("ERROR[to_file main]: fopen() failed for to_file");	// printf() may be redirected
		exit(1);
	}
	char buffer[MAX_LINE];
	while(fgets(buffer, sizeof(buffer), stdin)) {
		fprintf(to_file, "%s", buffer);
	}
	fclose(to_file);
	if (argv[2][0] != '/') {
		munmap (data, sizeof (struct dfshm));
    	close (shmfd);
	}	
}
