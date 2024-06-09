#include <fcntl.h>
#include <sys/mman.h>

#include "cmd.h" // includes <sys/types.h> etc.

/* Expected call format cd (~f) <dir_name> (<dir_path>) */
/* changes the directory to the given <dir_name> */
/* if flag is set to ~f, an absolute path must be given */
/* if for <dir_name> ".." is entered the shell goes back one directory */


//changes the path to dir_name
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
	inpu1 = "<dir_name>" = argv[2]
	input2 = "-" = argv[3]
*/
int main(int argc, char **argv) {
	if (argc != 4 || *argv[2] == '-' || *argv[3] != '-') {
		printf("Syntax error - expected format: cd <dir_name>\n"); // note: user input format differs from input format receivde from shell
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
	//checks if flag is set
	if (*argv[1] == '~') {
		char *cwd = argv[2];
		if(0 == access(cwd, F_OK)) {
			strcpy(data->current_working_dir, cwd);
			printf("Changed the cwd to: %s\n", data->current_working_dir);
		} else {
			printf("Directory does not exist!\n");
		}
	} else { // if no falg is set append dir_name to path
		char cwd[SHELL_PATH_MAX];
		pthread_mutex_lock(&shm_mutex_lock);	
		strcpy(cwd, data->current_working_dir);
		pthread_mutex_unlock(&shm_mutex_lock);	
		char *dir_name = argv[2];
		int n = strlen(dir_name);
		int m = strlen(cwd);
		//printf("%s, %d\n", dir_name, n);
		//printf("%s, %d\n", cwd, m);
		//char new_cwd[n+m+2];
		char new_cwd[SHELL_PATH_MAX];

		//If the first two inputs form argv[2] are dots('.'), then cd jumps back one directory.
		if (dir_name[0] == '.' && dir_name[1] == '.' && dir_name[2] == '\0') {
			int i = m;
			for (i = m - 1; cwd[i] != '/'; i--) {
			}
			for (int j = 0; j < i; j ++) {
				new_cwd[j] = cwd[j];
			}
			new_cwd[i] = '\0';
		} else { //opens the new directory
			for (int i = 0; i < m; i++) {
				new_cwd[i] = cwd[i];
			}
			//printf("%s\n", new_cwd);
			new_cwd[m] = '/';
			for (int j = 0; j < n; j++) {
				new_cwd[m+1+j] = dir_name[j];
			}
			new_cwd[n+m+1] = '\0';
		}
		//printf("%s\n", new_cwd);
		if(0 == access(new_cwd, F_OK)) {
			strcpy(data->current_working_dir, new_cwd);
			printf("Changed the cwd to: %s\n", data->current_working_dir);
		} else {
			printf("Directory does not exist!\n");
		}
	}
	
	return 0;
}
