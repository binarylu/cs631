#include <stdio.h>
#include <unistd.h>
#include <bsd/stdlib.h>
#include <fts.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include "options.h"
#include "prints.h"
#include "compare.h"


#define HANDLE_ERROR(msg)        \
do {                             \
    perror(msg);                 \
    exit(EXIT_FAILURE);          \
} while (/* CONSTCOND */ 0)

static void Usage();
static void traverse(int argc, char *argv[], unsigned int flag, int termwidth);
static void display(FTSENT *p, FTSENT *list, unsigned int flag, int termwidth);
static int compare(const FTSENT **a, const FTSENT **b);
static int (*sort)(const FTSENT *a, const FTSENT *b);
static void (*print)(_item_format *format, FTSENT *cur, unsigned int flag);

unsigned int flag;

int main(int argc, char *argv[])
{
    int opt;
    struct winsize win;
    int termwidth;

    static char *defaultPath[] = {".", NULL};

    setprogname(argv[0]);

    flag = 0;
    if (isatty(STDOUT_FILENO)) {
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &win) == 0 &&
                win.ws_col > 0)
            termwidth = win.ws_col;
        else
            termwidth = -1;
        SETFLAG_C(flag);
        SETFLAG_q(flag);
    } else {
        termwidth = -1;
        SETFLAG_w(flag);
        SETFLAG_1(flag);
    }

    while ((opt = getopt(argc, argv, "AaCcdFfhiklnqRrSstuwx1")) != -1) {
        switch (opt) {
            case 'f': SETFLAG_f(flag);
            case 'a': SETFLAG_a(flag);
            case 'A': SETFLAG_A(flag); break;
            case 'C': SETFLAG_C(flag); break;
            case 'c': SETFLAG_c(flag); break;
            case 'd': SETFLAG_d(flag); break;
            case 'F': SETFLAG_F(flag); break;
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

    sort = ISFLAG_r(flag) ? rsortByName : sortByName;
    if (ISFLAG_t(flag)) {
        sort = ISFLAG_r(flag) ? rsortByMtime : sortByMtime;
        if (ISFLAG_c(flag))
            sort = ISFLAG_r(flag) ? rsortByCtime : sortByCtime;
        else if (ISFLAG_u(flag))
            sort = ISFLAG_r(flag) ? rsortByAtime : sortByAtime;
    }
    if (ISFLAG_S(flag))
        sort = ISFLAG_r(flag) ? rsortBySize : sortBySize;

    if (ISFLAG_1(flag)) {
        print = print_1;
    } else if (ISFLAG_l(flag)) {
        print = print_l;
    } else if (ISFLAG_C(flag)) {
        print = print_C;
    }

    argc -= optind;
    argv += optind;
    if (argc)
        traverse(argc, argv, flag, termwidth);
    else
        traverse(1, defaultPath, flag, termwidth);
    return 0;
}

static void
Usage() {
    fprintf(stderr, "Usage: %s [ −AacCdFfhiklnqRrSstuwx1 ] [file ...]", getprogname());
    exit(EXIT_FAILURE);
}

static void
traverse(int argc, char *argv[], unsigned int flag, int termwidth) {
    FTS *ftsp;
    FTSENT *p, *chp;
    int fts_options;
    int ch_options;

    fts_options = getFtsOptions(flag);

    if ((ftsp = fts_open(argv, fts_options, ISFLAG_f(flag) ? NULL : compare)) == NULL)
        HANDLE_ERROR("fts_open failed");

    display(NULL, fts_children(ftsp, 0), flag, termwidth);
    if (ISFLAG_d(flag)) {
        fts_close(ftsp);
        return;
    }

    ch_options = !ISFLAG_R(flag) && fts_options & FTS_NOSTAT ? FTS_NAMEONLY : 0;
    while ((p = fts_read(ftsp)) != NULL) {
        switch (p->fts_info) {
            case FTS_D:
                if (p->fts_level != FTS_ROOTLEVEL &&
                        p->fts_name[0] == '.' && !ISFLAG_A(flag))
                    break;
                if ((chp = fts_children(ftsp, ch_options)) == NULL)
                    HANDLE_ERROR("fts_children failed");
                display(p, chp, flag, termwidth);
                if (!ISFLAG_R(flag) && chp != NULL)
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
display(FTSENT *p, FTSENT *list, unsigned int flag, int termwidth) {
    FTSENT *cur;
    _item_format format = {0};
    int len, blocksize, blocknum, size;
    int item_num = 0;
    blocksize = getBlockSize(flag);

    if (list == NULL)
        return;

    for (cur = list; cur; cur = cur->fts_link) {
        if (cur->fts_info == FTS_ERR || cur->fts_info == FTS_NS) {
            cur->fts_number = NO_PRINT;
            continue;
        }
        if (p == NULL) {
            /* Directories will be displayed later */
            if (cur->fts_info == FTS_D && !ISFLAG_d(flag)) {
                cur->fts_number = NO_PRINT;
                continue;
            }
        } else {
            if (cur->fts_name[0] == '.' && !ISFLAG_A(flag)){
                cur->fts_number = NO_PRINT;
                continue;
            }
        }
        if (cur->fts_namelen > format.maxlen_name)
            format.maxlen_name = cur->fts_namelen;
        if (NEEDSTATS(flag)) {
            if (cur->fts_statp == NULL) return;
            len = getNumLen(cur->fts_statp->st_nlink);
            if (len > format.maxlen_links)
                format.maxlen_links = len;
            
            if (ISFLAG_n(flag)) {
                len = getNumLen(cur->fts_statp->st_uid);
            } else {
                len = strlen(getUserNameByUID(cur->fts_statp->st_uid));
            }
            if (len > format.maxlen_owner)
                format.maxlen_owner = len;

            if (ISFLAG_n(flag)) {
                len = getNumLen(cur->fts_statp->st_gid);
            } else {
                len = strlen(getGroupNameByGID(cur->fts_statp->st_gid));
            }
            if (len > format.maxlen_group)
                format.maxlen_group = len;

            if (ISFLAG_h(flag)) {
                len = getHumanReadableLen(cur->fts_statp->st_size);
            } else {
                size = cur->fts_statp->st_size;
                if (ISFLAG_k(flag)) {
                    size = (int)ceil(size / 1024.0);
                }
                len = getNumLen(size);
            }
            if (len > format.maxlen_size)
                format.maxlen_size = len;

            blocknum = (int)ceil(cur->fts_statp->st_blocks * BLOCKSIZE / (double)blocksize);
            format.total_block += blocknum;

            if (ISFLAG_h(flag)) {
                len = getHumanReadableLen(blocknum);
            } else {
                len = getNumLen(blocknum);
            }
            if (len > format.maxlen_blocknum)
                format.maxlen_blocknum = len;

            len = getNumLen(cur->fts_statp->st_ino);
            if (len > format.maxlen_inode)
                format.maxlen_inode = len;
        }
        ++item_num;
    }
    format.item_num = item_num;
    format.termwidth = termwidth;
    print(&format, list, flag);
}

static int
compare(const FTSENT **a, const FTSENT **b) {
    int a_info, b_info;

    a_info = (*a)->fts_info;
    if (a_info == FTS_ERR)
        return 0;
    b_info = (*b)->fts_info;
    if (b_info == FTS_ERR)
        return 0;

    if (a_info == FTS_NS || b_info == FTS_NS) {
        if (b_info != FTS_NS)
            return 1;
        else if (a_info != FTS_NS)
            return -1;
        else
            return sortByName(*a, *b);
    }

    if (a_info != b_info && !ISFLAG_d(flag) && 
            (*a)->fts_level == FTS_ROOTLEVEL) {
        if (a_info == FTS_D)
            return 1;
        else if (b_info == FTS_D)
            return 0;
    }
    return sort(*a, *b);
}