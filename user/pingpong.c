#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int parent_to_child_pipe[2] = {0};
    int child_to_parent_pipe[2] = {0};
    char buf[1] = {"a"};
    if (pipe(parent_to_child_pipe) != 0 || pipe(child_to_parent_pipe) != 0) {
        printf("ERROR: Creating pipes failed!\n");
        exit(1);
    }

    if (fork() == 0) {
        close(parent_to_child_pipe[1]);
        close(child_to_parent_pipe[0]);

        char read_buf[1] = {"0"};

        if (read(parent_to_child_pipe[0], read_buf, 1) != 1 || read_buf[0] != buf[0]) {
            printf("ERROR: Child failed to read from parent!\n");
            exit(1);
        }
        int pid = getpid();
        printf("%d: received ping\n", pid);
        if (write(child_to_parent_pipe[1], buf, 1) != 1) {
            printf("ERROR: Child failed to write to parent!\n");
            exit(1);
        }
        exit(0);
    }

    close(parent_to_child_pipe[0]);
    close(child_to_parent_pipe[1]);

    char read_buf[1] = {"0"};
    if (write(parent_to_child_pipe[1], buf, 1) != 1) {
        printf("ERROR: Parent failed to write to child!\n");
        exit(1);
    }

    if (read(child_to_parent_pipe[0], read_buf, 1) != 1 || read_buf[0] != buf[0]) {
        printf("ERROR: Parent failed to read from child!\n");
        exit(1);
    }

    int pid = getpid();
    printf("%d: received pong\n", pid);
    exit(0);
}
