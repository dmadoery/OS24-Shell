#include <fcntl.h> // O_RDWR
#include <sys/mman.h>

#include "cmd.h"

#define RIGHTS 0777 // 0777: give read, write, and execute rights to owner, group, and others

/* Expected call format md <dir_name> */
/* creates a new directory called <dir_name> */

int md(const char *dir_name) {
	if (mkdir(dir_name, RIGHTS) == 0) {
		return 0;		
	} else {
		return errno;
	}
}

/*
receives input in the format
	command = "./md" = argv[0]
	flag = "-" = argv[1]
	inpu1 = "<dir_name>" = argv[2]
	input2 = "-" = argv[3]
*/
int main(int argc, char **argv) {
	if (argc != 4 || *argv[1] != '-' || *argv[2] == '-' || *argv[3] != '-') {
		printf("Syntax error - expected format: md <dir_name>\n"); // note: user input format differs from input format receivde from shell
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
	pthread_mutex_lock(&shm_mutex_lock);
	cwd = data->current_working_dir;
	pthread_mutex_unlock(&shm_mutex_lock);
	int m = strlen(cwd);
	char *dir_name = argv[2];
	int n = strlen(dir_name);
	char new_cwd[n+m+2];
	for (int i = 0; i < m; i++) {
				new_cwd[i] = cwd[i];
			}
			printf("%s\n", new_cwd);
			new_cwd[m] = '/';
			for (int j = 0; j < n; j++) {
				new_cwd[m+1+j] = dir_name[j];
			}
			new_cwd[n+m+1] = '\0';
	
	int ret = md(new_cwd);
	
	munmap (data, sizeof (struct dfshm));
    close (shmfd);
	if (ret != 0) {	// error handling
		switch (ret) {
			case 2:
				printf("(Parent) directory in %s not found\n", argv[2]);
				break;
			case 17:
				printf("Directory %s could not be created as it already exists\n", argv[2]);
				break;
			default:
				printf("Unspecified error [errno: %d]\n", ret);
		}			
		return -1;
	}		
	return 0;
}
