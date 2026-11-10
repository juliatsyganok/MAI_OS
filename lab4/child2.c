#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define MAX_LEN  4096
#define SHM_NAME "/lab4_shm"
#define SHM_SIZE 65536

typedef struct {
    int ready1;
    int ready2;
    int len1;
    int len2;
    char buf1[MAX_LEN];
    char buf2[MAX_LEN];
} SharedData;

static void reverse_string(char *s) {
    int n = (int)strlen(s);
    for (int i = 0; i < n / 2; i++) {
        char t = s[i];
        s[i] = s[n - 1 - i];
        s[n - 1 - i] = t;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { perror("open"); return 1; }

    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd < 0) { perror("shm_open child2"); return 1; }

    SharedData *shm = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm == MAP_FAILED) { perror("mmap child2"); return 1; }
    close(shm_fd);

    while (1) {
        while (shm->ready2 == 0) usleep(1000);

        if (shm->len2 == -1) break;

        reverse_string(shm->buf2);

        if (write(fd, shm->buf2, shm->len2) < 0) { perror("write"); break; }
        if (write(fd, "\n", 1) < 0) { perror("write nl"); break; }

        shm->ready2 = 0;
    }

    close(fd);
    munmap(shm, SHM_SIZE);
    return 0;
}