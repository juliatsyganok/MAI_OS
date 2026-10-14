#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

#define MAX_LEN   4096
#define THRESHOLD 10 

int main(void)
{
    char file1[256], file2[256];
    printf("Имя файла для child1: ");
    fflush(stdout);
    if (fgets(file1, sizeof(file1), stdin) == NULL) {
        fprintf(stderr, "Ошибка чтения\n");
        return 1;
    }
    file1[strcspn(file1, "\n")] = '\0';

    printf("Имя файла для child2: ");
    fflush(stdout);
    if (fgets(file2, sizeof(file2), stdin) == NULL) {
        fprintf(stderr, "Ошибка чтения\n");
        return 1;
    }
    file2[strcspn(file2, "\n")] = '\0';


    int pipe1[2], pipe2[2];
    if (pipe(pipe1) < 0) {
        perror("pipe1");
        return 1;
    }
    if (pipe(pipe2) < 0) {
        perror("pipe2");
        return 1;
    }
    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("fork child1");
        return 1;
    }

    if (pid1 == 0) {
        close(pipe1[1]);  
        close(pipe2[0]); 
        close(pipe2[1]);
        if (dup2(pipe1[0], STDIN_FILENO) < 0) {
            perror("dup2 child1");
            _exit(1);
        }
        close(pipe1[0]);
        char *argv[] = {"./child1", file1, NULL};
        char *envp[] = {NULL};
        execve("./child1", argv, envp);
        perror("execve child1");
        _exit(1);
    }
    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("fork child2");
        return 1;
    }

    if (pid2 == 0) {
        close(pipe2[1]);
        close(pipe1[0]);
        close(pipe1[1]);

        if (dup2(pipe2[0], STDIN_FILENO) < 0) {
            perror("dup2 child2");
            _exit(1);
        }
        close(pipe2[0]);

        char *argv[] = {"./child2", file2, NULL};
        char *envp[] = {NULL};
        execve("./child2", argv, envp);

        perror("execve child2");
        _exit(1);
    }
    close(pipe1[0]);
    close(pipe2[0]);
    printf("\nСтроки:\n");

    char buf[MAX_LEN];
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        size_t len = strlen(buf);
        int has_nl = (len > 0 && buf[len - 1] == '\n');
        if (has_nl) {
            buf[len - 1] = '\0';
            len--;
        }

        int target_fd;
        if ((int)len > THRESHOLD) {
            target_fd = pipe2[1];   
        } else {
            target_fd = pipe1[1];  
        }
        if (write(target_fd, buf, len) < 0) {
            perror("write body");
            break;
        }
        if (write(target_fd, "\n", 1) < 0) {
            perror("write nl");
            break;
        }
    }
    close(pipe1[1]);
    close(pipe2[1]);
    int status;
    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);

    printf("\nЗапсиано %s, %s\n", file1, file2);
    return 0;
}