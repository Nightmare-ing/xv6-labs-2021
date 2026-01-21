#include "kernel/types.h"
#include "user/user.h"

void sieve_prime(int fd);

int main(int argc, char *argv[]) {
    // close unrelated fds, no read from stdin
    close(0);
    close(2);

    int p[2] = {0};
    if (pipe(p) != 0) {
        printf("ERROR: creating pipe failed!\n");
    }

    if (fork() == 0) {
        // child only use the read end
        close(p[1]);

        sieve_prime(p[0]);
        exit(0);
    }

    // parent only use the write end
    close(p[0]);

    printf("prime %d\n", 2);
    for (uint32 i = 3; i <= 35; ++i) {
        if (i % 2 != 0 && write(p[1], &i, 4) != 4) {
            printf("ERROR: failed to write to pipe!\n");
            exit(-1);
        }
    }
    // close pipe when finish writing
    close(p[1]);

    int child_state = 0;
    wait(&child_state);
    if (child_state != 0) {
        printf("ERROR: child is in abnormal state!\n");
        exit(-1);
    }
    exit(0);
}

// read datas from `fd`, and sieve primes for it
void sieve_prime(int fd) {
    uint32 prime = 0;
    if (read(fd, &prime, 4) != 4) {
        return;
    }
    printf("prime %d\n", prime);

    int p[2] = {0};
    if (pipe(p) != 0) {
        printf("ERROR: failed to create pipe!\n");
    }

    if (fork() == 0) {
        close(p[1]);
        sieve_prime(p[0]);
        exit(0);
    }

    close(p[0]);
    int num = 0;
    while(read(fd, &num, 4) != 0) {
        if (num % prime != 0 && write(p[1], &num, 4) != 4) {
            printf("ERROR: failed to write to pipe!\n");
            exit(-1);
        }
    }
    close(p[1]);

    int child_state = 0;
    wait(&child_state);
    if (child_state != 0) {
        printf("ERROR: child is in abnormal state!\n");
        exit(-1);
    }
    return;
}

