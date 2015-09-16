#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#ifndef BUFFSIZE
#define BUFFSIZE 10240
#endif

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

int getfilename(char *path, char **filename) {
    char *p = strrchr(path, '/');
    if (p == NULL) {
        *filename = path;
    } else {
        *filename = p + 1;
    }
    return 0;
}

#if 0
int main(int argc, char *argv[])
{
    char *b;
    getfilename(argv[1], &b);
    printf("%s\n", b);
    return 0;
}
#endif

#if 1
int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Wrong arguments!\nUsage: ./tcp source target\n");
        exit(1);
    }

    int fdr, fdw, fddir;
    char *src = argv[1];
    char *dst = argv[2];

    umask(0);

    if ((fddir = open(src, O_RDONLY | O_DIRECTORY)) != -1) {
        fprintf(stderr, "Source file cannot be a directory\n");
        close(fddir);
        exit(1);
    }

    if ((fdr = open(src, O_RDONLY)) == -1) {
        fprintf(stderr, "Unable to open source file '%s': %s\n", src, strerror(errno));
        exit(2);
    }

    if ((fddir = open(dst, O_RDONLY | O_DIRECTORY)) == -1) {
        if (dst[strlen(dst) - 1] == '/') {
            fprintf(stderr, "cannot create regular file '%s': Not a directory\n", dst);
            exit(2);
        }
        if ((fdw = open(dst, O_WRONLY | O_CREAT | O_TRUNC,  S_IRUSR | S_IWUSR)) == -1) {
            fprintf(stderr, "Unable to open target file '%s': %s\n", dst, strerror(errno));
            exit(2);
        }
    } else {
        close(fddir);
        char target[PATH_MAX];
        char *filename;
        getfilename(src, &filename);
        snprintf(target, PATH_MAX, "%s/%s", dst, filename);
        if ((fdw = open(target, O_WRONLY | O_CREAT | O_TRUNC,  S_IRUSR | S_IWUSR)) == -1) {
            fprintf(stderr, "Unable to open target file '%s': %s\n", dst, strerror(errno));
            exit(2);
        }
    }

    int n;
    char buf[BUFFSIZE];

    while ((n = read(fdr, buf, BUFFSIZE)) > 0) {
        if (write(fdw, buf, n) != n) {
            fprintf(stderr, "Copy failed!\n");
            exit(3);
        }
    }
    if (n < 0) {
        fprintf(stderr, "read error\n");
        exit(3);
    }

    close(fdr);
    close(fdw);
    return 0;
}
#endif