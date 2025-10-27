#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("should give an argument, usage: sleep <integer>\n");
        exit(1);
    }
    int sleep_time = atoi(argv[1]);
    if (sleep(sleep_time) != 0) {
        printf("Sleep failed!\n");
    }
    exit(0);
}

