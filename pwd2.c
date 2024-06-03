#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>

#include "cmd.h"

/*
receives input in the format
	command = "./pwd2" = argv[0]
	flag = "-" = argv[1]
	inpu1 = "-" = argv[2]
	input2 = "-" = argv[3]
*/
int main(int argc, char **argv) {
	if (argc != 4 || *argv[1] != '-' || *argv[2] != '-' || *argv[3] != '-') {
		printf("Syntax error - expected format: pwd2 <dir_name>\n"); // note: user input format differs from input format receivde from shell
		return -1;
	}
	//struct dfshm *data;
	char *cwd;
	int shmfd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR);
	if (shmfd == -1) {
		printf("ERROR[pwd2] shm_open failed\n");
		return -1;
	}
	struct dfshm *data = mmap(NULL, sizeof(struct dfshm), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
	if (data == NULL) {
		printf("ERROR[pwd2] mmap failed\n");
		return -1;
	}
	cwd = data->current_working_dir;
	printf("cwd: %s\n", cwd);
	munmap (data, sizeof (struct dfshm));
    close (shmfd);
}
