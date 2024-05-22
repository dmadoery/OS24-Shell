#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>	// chdir
#include <fcntl.h>
#include <sys/mman.h>
#include "cmd.h"

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
    int shmfd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR);
	if (shmfd == -1) {
		printf("[cd] shm_open failed\n");
	}
	struct dfshm *data = mmap(NULL, sizeof(struct dfshm), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
	if (data == NULL) {
		printf("[cd] mmap failed\n");
	}
	char cwd[PATH_MAX];
	strcpy(cwd, data->current_working_dir);
	char *dir_name = argv[2];
	int n = strlen(dir_name);
	int m = strlen(cwd);
	printf("%s, %d\n", dir_name, n);
	printf("%s, %d\n", cwd, m);
	//char new_cwd[n+m+2];
	char new_cwd[PATH_MAX];
	printf("strlen(new_cwd): %ld\n", strlen(new_cwd));
	for (int i = 0; i < m; i++) {
		new_cwd[i] = cwd[i];
	}
	//printf("%s\n", new_cwd);
	new_cwd[m] = '/';
	for (int j = 0; j < n; j++) {
		new_cwd[m+1+j] = dir_name[j];
	}
	new_cwd[n+m+2] = '\0';
	printf("%s\n", new_cwd);
	if(0 == access(new_cwd, F_OK)) {
		strcpy(data->current_working_dir, new_cwd);
		printf("Changed the cwd to: %s\n", data->current_working_dir);
	} else {
		printf("Directory does not exist!\n");
	}
	
	return 0;
}
