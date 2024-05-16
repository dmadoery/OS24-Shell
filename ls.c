#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "cmd.h"

//https://iq.opengenus.org/ls-command-in-c/#google_vignette

/* Expected call format ls (~a) */
int ls(int flag) {
	struct dirent *d;
    char *cwd;
    int shmfd = shm_open("/Open_SHM4", O_RDWR, S_IRUSR | S_IWUSR);
	if (shmfd == -1) {
		printf("[pwd2] shm_open failed\n");
	}
	struct dfshm *data = mmap(NULL, sizeof(struct dfshm), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
	if (data == NULL) {
		printf("[pwd2] mmap failed\n");
	}
	cwd = data->current_working_dir;
    DIR *dh = opendir(cwd);
    if (!dh) {
        if (errno = ENOENT) {
            perror("Directory doesen't exist\n");
        }
        else
        {
            perror("Unable to read directory\n");
        }
        exit(EXIT_FAILURE);
    }
    while ((d = readdir(dh)) != NULL) {
		//If hidden files are found we continue dont know if hidden files is working...
		if (!flag && d->d_name[0] == '.')
			continue;
		printf("%s  ", d->d_name);   
        if (flag && d->d_name[0] == '.') 
            printf("%s  ", d->d_name);
	}
    printf("\n");
}

int main(int argc, char **argv) {
    int flag;
    if(*argv[1] == '-') {
        flag = 0;
        ls(flag);
    } else if (strcmp(argv[1], "~a") == 0){
        flag = 1;
        ls(flag);
    }
	return 0;
}