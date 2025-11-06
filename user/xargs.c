#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

char *cpy_str(char *source);

int main(int argc, char *argv[]) {
    char *args[MAXARG];

    // copy the arguments except xargs from argv to args
    for (int i = 1; i < argc; ++i) {
        char *copy;
        if ((copy = cpy_str(argv[i])) != 0) {
            args[i - 1] = copy;
        }
    }


    char buf[512], letter;
    char *p = buf;
    int num_args = argc - 1;

    // read arguments from stdin
    while (read(0, &letter, 1) == 1) {
        if (letter == ' ' || letter == '\n') {
            *p = '\0';
            char *copy;
            if ((copy = cpy_str(buf)) != 0) {
                args[num_args] = copy;
                ++num_args;
            }

            if (num_args > MAXARG) {
                fprintf(2, "xargs: too many arguments\n");
                exit(1);
            }

            if (letter == '\n') {
                args[num_args] = 0;

                // in child process, execute the command
                if (fork() == 0) {
                    exec(args[0], args);
                    exit(1);
                } 

                int status = 0;
                wait(&status);
                if (status == 1) {
                    fprintf(2, "xargs: failed to exec\n");
                    exit(1);
                }

                // free allocated memory
                for (int i = argc; i < num_args; ++i) {
                    free(args[i]);
                }

                // reset the number of args
                num_args = argc - 1;
            }

            // clear the buffer
            p = buf;
            continue;
        }
            
        // put the letter into the buffer
        *p++ = letter;
    }
    exit(0);
}

// allocate memory and copy the string, return the address of the allocated
// memory
char *cpy_str(char *source) {
    int len = strlen(source);
    if (len > 0) {
        char *copy = malloc(len + 1);
        memmove(copy, source, len + 1);
        return copy;
    } else {
        return 0;
    }
}

