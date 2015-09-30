#include <stdio.h>
#include <unistd.h>
#include <bsd/stdlib.h>
#include <fts.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#define ISFLAG_A(f) (f)&0x00000001
#define ISFLAG_a(f) (f)&0x00000002
#define ISFLAG_C(f) (f)&0x00000004
#define ISFLAG_c(f) (f)&0x00000008
#define ISFLAG_d(f) (f)&0x00000010
#define ISFLAG_F(f) (f)&0x00000020
#define ISFLAG_f(f) (f)&0x00000040
#define ISFLAG_h(f) (f)&0x00000080
#define ISFLAG_i(f) (f)&0x00000100
#define ISFLAG_k(f) (f)&0x00000200
#define ISFLAG_l(f) (f)&0x00000400
#define ISFLAG_n(f) (f)&0x00000800
#define ISFLAG_q(f) (f)&0x00001000
#define ISFLAG_R(f) (f)&0x00002000
#define ISFLAG_r(f) (f)&0x00004000
#define ISFLAG_S(f) (f)&0x00008000
#define ISFLAG_s(f) (f)&0x00010000
#define ISFLAG_t(f) (f)&0x00020000
#define ISFLAG_u(f) (f)&0x00040000
#define ISFLAG_w(f) (f)&0x00080000
#define ISFLAG_x(f) (f)&0x00100000
#define ISFLAG_1(f) (f)&0x00200000

#define SETFLAG_A(f) do { (f)|=0x00000001; } while(0)
#define SETFLAG_a(f) do { (f)|=0x00000002; } while(0)
#define SETFLAG_C(f) do { (f)|=0x00000004; } while(0)
#define SETFLAG_c(f) do { (f)|=0x00000008; } while(0)
#define SETFLAG_d(f) do { (f)|=0x00000010; } while(0)
#define SETFLAG_F(f) do { (f)|=0x00000020; } while(0)
#define SETFLAG_f(f) do { (f)|=0x00000040; } while(0)
#define SETFLAG_h(f) do { (f)|=0x00000080; } while(0)
#define SETFLAG_i(f) do { (f)|=0x00000100; } while(0)
#define SETFLAG_k(f) do { (f)|=0x00000200; } while(0)
#define SETFLAG_l(f) do { (f)|=0x00000400; } while(0)
#define SETFLAG_n(f) do { (f)|=0x00000800; } while(0)
#define SETFLAG_q(f) do { (f)|=0x00001000; } while(0)
#define SETFLAG_R(f) do { (f)|=0x00002000; } while(0)
#define SETFLAG_r(f) do { (f)|=0x00004000; } while(0)
#define SETFLAG_S(f) do { (f)|=0x00008000; } while(0)
#define SETFLAG_s(f) do { (f)|=0x00010000; } while(0)
#define SETFLAG_t(f) do { (f)|=0x00020000; } while(0)
#define SETFLAG_u(f) do { (f)|=0x00040000; } while(0)
#define SETFLAG_w(f) do { (f)|=0x00080000; } while(0)
#define SETFLAG_x(f) do { (f)|=0x00100000; } while(0)
#define SETFLAG_1(f) do { (f)|=0x00200000; } while(0)

#define HANDLE_ERROR(msg)        \
do {                             \
    perror(msg);                 \
    exit(EXIT_FAILURE);          \
} while (/* CONSTCOND */ 0)

static void Usage();
static void traverse(int argc, char *argv[], int options);
static void display(FTSENT *p, FTSENT *list);
static int compar(const FTSENT **a, const FTSENT **b);
static int (*sort)(const FTSENT *a, const FTSENT *b);
static int sortByName(const FTSENT *a, const FTSENT *b);

int main(int argc, char *argv[])
{
    int opt;
    unsigned int flag;

    static char *defaultPath[] = {".", NULL};

    setprogname(argv[0]);

    /* temporary */
    sort = sortByName;

    flag = 0;
    while ((opt = getopt(argc, argv, "AaCcdFfhiklnqRrSstuwx1")) != -1) {
        switch (opt) {
            case 'A': SETFLAG_A(flag); break;
            case 'a': SETFLAG_a(flag); break;
            case 'C': SETFLAG_C(flag); break;
            case 'c': SETFLAG_c(flag); break;
            case 'd': SETFLAG_d(flag); break;
            case 'F': SETFLAG_F(flag); break;
            case 'f': SETFLAG_f(flag); break;
            case 'h': SETFLAG_h(flag); break;
            case 'i': SETFLAG_i(flag); break;
            case 'k': SETFLAG_k(flag); break;
            case 'l': SETFLAG_l(flag); break;
            case 'n': SETFLAG_n(flag); break;
            case 'q': SETFLAG_q(flag); break;
            case 'R': SETFLAG_R(flag); break;
            case 'r': SETFLAG_r(flag); break;
            case 'S': SETFLAG_S(flag); break;
            case 's': SETFLAG_s(flag); break;
            case 't': SETFLAG_t(flag); break;
            case 'u': SETFLAG_u(flag); break;
            case 'w': SETFLAG_w(flag); break;
            case 'x': SETFLAG_x(flag); break;
            case '1': SETFLAG_1(flag); break;
            default: Usage();
        }
    }
    int fts_options;
    fts_options = FTS_PHYSICAL;
    argc -= optind;
    argv += optind;
    if (argc)
        traverse(argc, argv, fts_options);
    else
        traverse(1, defaultPath, fts_options);
    return 0;
}

static void
Usage() {
    fprintf(stderr, "Usage: %s [ −AacCdFfhiklnqRrSstuwx1 ] [file ...]", getprogname());
    exit(EXIT_FAILURE);
}

static void
traverse(int argc, char *argv[], int options) {
    FTS *ftsp;
    FTSENT *p, *chp;
    int ch_options;

    ch_options = FTS_NAMEONLY;

    if ((ftsp = fts_open(argv, options, compar)) == NULL)
        HANDLE_ERROR("fts_open failed");

    while ((p = fts_read(ftsp)) != NULL) {
        switch (p->fts_info) {
            case FTS_D:
                if ((chp = fts_children(ftsp, ch_options)) == NULL)
                    HANDLE_ERROR("fts_children failed");
                display(p, chp);
                fts_set(ftsp, p, FTS_SKIP);
                break;
        }
    }
    if (errno == 0)
        fts_close(ftsp);
    else {
        fts_close(ftsp);
        HANDLE_ERROR("fts_read failed");
    }
}

static void
display(FTSENT *p, FTSENT *list) {
    FTSENT *cur;
    for (cur = list; cur; cur = cur->fts_link) {
        if (cur->fts_name[0] == '.')
            continue;
        printf("%s  ", cur->fts_name);
    }
    printf("\n");
}

static int
compar(const FTSENT **a, const FTSENT **b) {
    return sort(*a, *b);
}

static int
sortByName(const FTSENT *a, const FTSENT *b) {
    return (strcmp(a->fts_name, b->fts_name));
}