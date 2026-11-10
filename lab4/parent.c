#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>

#define MAX_LEN   4096
#define THRESHOLD 10
#define SHM_NAME  "/lab4_shm"
#define SHM_SIZE  65536

typedef struct {
    int ready1;
    int ready2;
    int len1;
    int len2;
    char buf1[MAX_LEN];
    char buf2[MAX_LEN];
} SharedData;

int main(void) {
    char file1[256], file2[256];

    printf("Файл для child1: ");
    fflush(stdout);
    if (fgets(file1, sizeof(file1), stdin) == NULL) { perror("fgets"); return 1; }
    file1[strcspn(file1, "\n")] = '\0';

    printf("Файл для child2: ");
    fflush(stdout);
    if (fgets(file2, sizeof(file2), stdin) == NULL) { perror("fgets"); return 1; }
    file2[strcspn(file2, "\n")] = '\0';

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) { perror("shm_open"); return 1; }

    if (ftruncate(shm_fd, SHM_SIZE) < 0) {
        perror("ftruncate");
        close(shm_fd);
        shm_unlink(SHM_NAME);
        return 1;
    }

    SharedData *shm = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        shm_unlink(SHM_NAME);
        return 1;
    }
    close(shm_fd);
    memset(shm, 0, sizeof(SharedData));

    pid_t pid1 = fork();
    if (pid1 < 0) { perror("fork child1"); return 1; }
    if (pid1 == 0) {
        char *argv[] = {"./child1", file1, NULL};
        char *envp[] = {NULL};
        execve("./child1", argv, envp);
        perror("execve child1");
        _exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 < 0) { perror("fork child2"); return 1; }
    if (pid2 == 0) {
        char *argv[] = {"./child2", file2, NULL};
        char *envp[] = {NULL};
        execve("./child2", argv, envp);
        perror("execve child2");
        _exit(1);
    }

    printf("\nВводите строки:\n");
    char buf[MAX_LEN];
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        size_t len = strlen(buf);
        int has_nl = (len > 0 && buf[len - 1] == '\n');
        if (has_nl) { buf[len - 1] = '\0'; len--; }

        if ((int)len > THRESHOLD) {
            while (shm->ready2 != 0) usleep(1000);
            strncpy(shm->buf2, buf, MAX_LEN - 1);
            shm->buf2[MAX_LEN - 1] = '\0';
            shm->len2 = (int)len;
            shm->ready2 = 1;
        } else {
            while (shm->ready1 != 0) usleep(1000);
            strncpy(shm->buf1, buf, MAX_LEN - 1);
            shm->buf1[MAX_LEN - 1] = '\0';
            shm->len1 = (int)len;
            shm->ready1 = 1;
        }
    }

    while (shm->ready1 != 0 || shm->ready2 != 0) usleep(1000);

    shm->len1 = -1; shm->ready1 = 1;
    shm->len2 = -1; shm->ready2 = 1;

    int status;
    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);

    munmap(shm, SHM_SIZE);
    shm_unlink(SHM_NAME);
    return 0;
}