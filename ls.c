#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "cmd.h"

//https://iq.opengenus.org/ls-command-in-c/#google_vignette

/* Expected call format ls (~a) */
/* lists all (not hidden) directories */
/* if flag ~a is set it lists the hidden directories too */

int ls(int flag) {
	struct dirent *d;
    char *cwd;

    //Get current directory path
    int shmfd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR);
	if (shmfd == -1) {
		printf("[ls] shm_open failed\n");
	}
	struct dfshm *data = mmap(NULL, sizeof(struct dfshm), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
	if (data == NULL) {
		printf("[ls] mmap failed\n");
	}
	pthread_mutex_lock(&shm_mutex_lock);
	cwd = data->current_working_dir;
	pthread_mutex_unlock(&shm_mutex_lock);

    //Open directory
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
    //Read all files in directory and print them on the shell 
    while ((d = readdir(dh)) != NULL) {
		if (!flag && d->d_name[0] == '.') //if flag is not set print without hidden files
			continue;
		printf("%s  ", d->d_name);   
        if (flag && d->d_name[0] == '.') //if flag is set print with hidden files
            printf("%s  ", d->d_name);
	}
    printf("\n");
}

int main(int argc, char **argv) {
    if (argc != 4 || *argv[2] != '-' || *argv[3] != '-') {
		printf("Syntax error - expected format: ls (~a)\n"); // note: user input format differs from input format receivde from shell
		return -1;
	}
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
