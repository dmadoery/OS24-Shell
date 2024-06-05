#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "cmd.h" // PATH_MAX

#define MAX_LINE 128

/*
receives input in the format
	command = "./cp" = argv[0]
	flag = "-" = argv[1]
	inpu1 = "<from_file>" = argv[2]
	input2 = "<to_file>" = argv[3]
*/
int main(int argc, char **argv) {
	if (argc != 4 || *argv[1] != '-' || *argv[2] == '-' || *argv[3] == '-') {
		printf("Syntax error - expected format: cp <from_file> <[to_path/]to_file>\n"); // note: user input format differs from input format receivde from shell
		return -1;
	}
	// open shared memory
	char *cwd;
	int shmfd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR);
	if (shmfd == -1) {
		printf("ERROR[cp pwd2]: shm_open failed\n");
		return -1;
	}
	struct dfshm *data = mmap(NULL, sizeof(struct dfshm), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
	if (data == NULL) {
		printf("ERROR[cp pwd2]: mmap failed\n");
		return -1;
	}
	pthread_mutex_lock(&shm_mutex_lock);
	cwd = data->current_working_dir;
	pthread_mutex_unlock(&shm_mutex_lock);
	
	int m = strlen(cwd);
	int n = strlen(argv[2]);
	char from_path_file[m + n + 2];
	
	for (int i = 0; i < m; i++) {
		from_path_file[i] = cwd[i];
	}
	from_path_file[m] = '/';
	for (int i = 0; i < n; i++) {
		from_path_file[m + 1 + i] = argv[2][i];
	}
	from_path_file[m + n + 1] = '\0';
	
	if (argv[3][0] == '/') {	// if the to_path begins with '/', then it is an absolute path
		m = 0;
	}
	n = strlen(argv[3]);
	char to_path_file[m + n + 2];
	
	for (int i = 0; i < m; i++) {
		to_path_file[i] = cwd[i];
	}
	to_path_file[m] = '/';
	for (int i = 0; i < n; i++) {
		to_path_file[m + 1 + i] = argv[3][i];
	}
	to_path_file[m + n + 1] = '\0';
	
	// open files
	FILE *from_file;
	FILE *to_file;
	from_file = fopen(from_path_file, "r");
	if (from_file == NULL) {
		printf("ERROR[cp main]: fopen() failed for from_file\n");	// printf() may be redirected
		return -1;
	}
	to_file = fopen(to_path_file, "w");
	if (to_file == NULL) {
		printf("ERROR[cp main]: fopen() failed for to_file\n");	// printf() may be redirected
		return -1;
	}
	char line[MAX_LINE];
	while (fgets(line, sizeof(line), from_file)) {
		fprintf(to_file, "%s", line);
	}
	fclose(from_file);
	fclose(to_file);
		
	munmap (data, sizeof (struct dfshm));
    close (shmfd);
	
}
