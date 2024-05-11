#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>

#define PATH_MAX 1024

/* Expected call format ls (~a) */
int ls(char flag) {
	struct dirent *d;
    DIR *dh = opendir(".");
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
		if (flag == 1 && d->d_name[0] == '.')
			continue;
		printf("%s  ", d->d_name);    
	}
    printf("\n");
}

int main(int argc, char **argv) {
    int flag;
    if(argv[1] != '-') {
        flag = 0;
        ls(flag);
    } else {
        flag = 1;
        ls(flag);
    }
	return 0;
}