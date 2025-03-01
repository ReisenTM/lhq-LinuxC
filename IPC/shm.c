#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define MEMSIZE 1024
static pid_t pid;
static void *ptr;
int main(int argc, char *argv[]) {
  int shmid = shmget(0, MEMSIZE, 0);
  pid = fork();
  if (pid < 0) {
    perror("fork()");
    exit(EXIT_FAILURE);
  } else if (pid == 0) { /* child process*/
    ptr = shmat(shmid, NULL, 0);
    if (ptr == (void *)-1) {
      perror("shmat");
      exit(EXIT_FAILURE);
    }
    strcpy((char *)ptr, "this is process");
    shmdt(ptr);
    exit(EXIT_SUCCESS);

  } else { //*parent process*/
    wait(NULL);
    ptr = shmat(shmid, NULL, 0);
    if (ptr == (void *)-1) {
      perror("shmat");
      exit(EXIT_FAILURE);
    }
    puts(ptr);
    shmdt(ptr);
    shmctl(shmid, IPC_RMID, 0);
    exit(EXIT_SUCCESS);
  }

  return EXIT_SUCCESS;
}
