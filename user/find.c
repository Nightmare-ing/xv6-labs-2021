#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/fs.h"

void search_dir(char *path, char *filename);

int main(int argc, char *argv[]) {
    search_dir(argv[1], argv[2]);
    exit(0);
}

void search_dir(char *path, char *filename) {
    int fd = 0;
    struct stat st;
    struct dirent de;
    char buf[512];
    
    // error handling
    if ((fd = open(path, 0)) == -1) {
        fprintf(2, "find: cannot open %s\n", path);
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if (st.type != T_DIR) {
        printf("find: %s is not a dir\n", path);
        close(fd);
        return;
    }

    // read the directory
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) {
            continue;
        }

        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
            continue;
        }

        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
            printf("find: path too long\n");
        }
        strcpy(buf, path);
        char *p = buf + strlen(path);
        *p++ = '/';

        // every filename has DIRSIZ bytes to store, which is the max size of a
        // file name
        memmove(p, de.name, DIRSIZ);
        
        // check the type of the item in the directory
        if (stat(buf, &st) < 0) {
            printf("find: cannot stat %s\n", buf);
            return;
        }

        if (st.type == T_DIR) {
            search_dir(buf, filename);
        } else if (strcmp(de.name, filename) == 0) {
            printf("%s\n", buf);
        }
    }
    close(fd);
}

