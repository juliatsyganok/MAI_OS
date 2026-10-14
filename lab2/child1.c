#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_LEN 4096

static void reverse_string(char *s)
{
    int n = (int)strlen(s);
    for (int i = 0; i < n / 2; i++) {
        char t = s[i];
        s[i] = s[n - 1 - i];
        s[n - 1 - i] = t;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        return 1;
    }

    int out_fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd < 0) {
        perror("open");
        return 1;
    }

    char buf[MAX_LEN];
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        size_t len = strlen(buf);

        int has_nl = (len > 0 && buf[len - 1] == '\n');
        if (has_nl) {
            buf[len - 1] = '\0';
        }

        reverse_string(buf);

        if (write(out_fd, buf, strlen(buf)) < 0) {
            perror("write");
            break;
        }
        if (has_nl) {
            if (write(out_fd, "\n", 1) < 0) {
                perror("write nl");
                break;
            }
        }
    }

    close(out_fd);
    return 0;
}