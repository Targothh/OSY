#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <stdlib.h>

#define ERROR_CODE 2

volatile int gen_kill = 0;

void sigterm_handler(int signal){
    gen_kill = 1;
}

int main (void){
    int gen_pid, nsd_pid;
    int status;
    int error_check = 0;
    int pipefd[2];
    if (pipe(pipefd) != 0) {
        return ERROR_CODE;
    } 
    if ((gen_pid = fork()) == -1){
        return ERROR_CODE;
    }
    if (gen_pid == 0){ //GEN code
        signal(SIGTERM, sigterm_handler);
        close(pipefd[0]);
        if (dup2(pipefd[1], 1) == -1) {
            return ERROR_CODE;
        }
        close(pipefd[1]);
        srand(time(NULL));
        while (!gen_kill) {
            printf("%d %d\n", rand() % 4096, rand() % 4096);
            fflush(stdout);
            sleep(1);
        }
        fprintf(stderr, "GEN TERMINATED\n");
        return 0;

    } else {
        if ((nsd_pid = fork()) == -1){
            return ERROR_CODE;
        }
        if (nsd_pid == 0){ //NSD code
            close(pipefd[1]);
            if (dup2(pipefd[0], 0) == -1) {
            return ERROR_CODE;
            }
            close(pipefd[0]);
            if (execl("nsd", "nsd", NULL) == -1){
                return ERROR_CODE;
            }
        } else { // MAIN code
            close(pipefd[0]);
            close(pipefd[1]);
            sleep(5);
            kill(gen_pid, SIGTERM);
            if (waitpid(gen_pid, &status,0) == -1){
                return ERROR_CODE;
            }
            if (WIFEXITED(status) != 1 || WEXITSTATUS(status) != 0){
                error_check = 1;
            }

            if (waitpid(nsd_pid, &status,0) == -1){
                return ERROR_CODE;
            }
            if (WIFEXITED(status) != 1 || WEXITSTATUS(status) != 0){
                error_check = 1;
            }
            if (error_check){
                printf("ERROR\n");
                exit(1);
            } else {
                printf("OK\n");
                exit(0);
            }
        }
    }
}