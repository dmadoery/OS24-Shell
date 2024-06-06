#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PATH_MAX 1024

// shared memory
static const char SHM_NAME[] = "/Open_SHM";

// mutex
pthread_mutex_t shm_mutex_lock;

struct cmd {
    char *command;
    char *flag;
    char *input1;
    char *input2;
};

//data for shared memory
struct dfshm {
	char current_working_dir[PATH_MAX];
	char pc_name[PATH_MAX];
};

