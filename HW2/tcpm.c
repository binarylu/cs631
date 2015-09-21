/*
 * "Advanced Programming in the UNIX Environment" Homework #2
 * Description: The tcpm utility copies the contents of the source to target.
 * Version 1.0
 * Date: 2015/09/21
 * Email: xlu9@stevens.edu
 * Copyright: Xiakun Lu
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <bsd/stdlib.h>
#include <string.h>
#include <errno.h>

/*
 * Get filename from the given path. Variable "path" is an input argument which a
 * an absolute or relative path. Variable "filename" is an output argument. The
 * function find the last '/' and make variable "filename" point to the next
 * character of the last '/'. If '/' does not exist in path, then "filename"
 * point to the first character of "path".
 */
static void getfilename(const char *path, const char **filename);

#define HANDLE_ERROR(msg)        \
do {                             \
    perror(msg);                 \
    exit(EXIT_FAILURE);          \
} while (/* CONSTCOND */ 0)

/*
 * Read the source file and copy its content to the target file. Source file can
 * not be a directory.
 */
int
main(int argc, char *argv[])
{
    int fdr, fdw, fddir;
    int n, file_size;

    char *src = argv[1];
    char *dst = argv[2];
    char *addr;
    struct stat st_src, st_dst;

    setprogname(argv[0]);

    if (argc != 3) {
        fprintf(stderr, "Wrong argument number!\n");
        exit(EXIT_FAILURE);
    }
    
    /* Source file can not be a directory */
    if (stat(src, &st_src) < 0)
        HANDLE_ERROR("Fail to access SOURCE");
    if (S_ISDIR(st_src.st_mode)) {
        fprintf(stderr, "SOURCE cannot be a directory\n");
        exit(EXIT_FAILURE);
    }

    /* Open the source file as a regular file */
    if ((fdr = open(src, O_RDONLY)) == -1)
        HANDLE_ERROR("Fail to read SOURCE");

    /*
     * Create a new file with the specified name that user give if the target
     * file is a regular file or does not exist. If the target is a directory,
     * createa new file named the same name with source file under this
     * directory. 
     */
    if ((fddir = open(dst, O_RDONLY | O_DIRECTORY)) == -1) {
        if (dst[strlen(dst) - 1] == '/') {
            /*
             * If the target file name followed by '/', must open it as a
             * directory.
             */
            fprintf(stderr, "cannot create regular file '%s': Not a directory\n", dst);
            exit(EXIT_FAILURE);
        }
        if ((fdw = open(dst, O_WRONLY | O_CREAT | O_TRUNC,  S_IRUSR | S_IWUSR)) == -1) {
            /*
             * If the target is not a directory and not followed by '/', create
             * it.
             */
            fprintf(stderr, "Unable to open target file '%s': %s\n", dst, strerror(errno));
            exit(EXIT_FAILURE);
        }
    } else {
        /*
         * The target is a directory if it can be opened as a directory.
         */
        close(fddir);
        const char *filename;
        /* Generate a new filename combining path and filename */
        getfilename(src, &filename);
        strcat(dst, "/");
        strcat(dst, filename);
        if ((fdw = open(dst, O_WRONLY | O_CREAT | O_TRUNC,  S_IRUSR | S_IWUSR)) == -1) {
            fprintf(stderr, "Unable to open target file '%s': %s\n", dst, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    if (fstat(fdw, &st_dst) < 0)
        HANDLE_ERROR("Fail to access TARGET");
    /* Source file and target file can not be the same files. */
    if (st_src.st_ino == st_dst.st_ino) {
        fprintf(stderr, "'%s' and '%s' are the same file\n", src , dst);
    }

    /* map the source file content to the memory */
    file_size = st_src.st_size;
    addr = (char *)mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fdr, 0);
    if (addr == MAP_FAILED) {
        close(fdr);
        close(fdw);
        HANDLE_ERROR("mmap error");
    }
    n = write(fdw, addr, file_size);
    if (n != file_size) {
        if (n == -1) {
            HANDLE_ERROR("Write failed");
        }
        fprintf(stderr, "partial write\n");
    }

    /* unmap */
    if (munmap(addr, file_size) == -1)
        fprintf(stderr, "munmap failed\n");

    close(fdr);
    close(fdw);
    return 0;
}

void
getfilename(const char *path, const char **filename)
{
    const char *p;
    
    /* Find the last '/' in variable path*/
    p = strrchr(path, '/');
    if (p == NULL) {
        *filename = path;
    } else {
        *filename = p + 1;
    }
}