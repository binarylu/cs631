#include "prints.h"

int
getNumLen(long long num) {
    int count = 1;
    while (num/=10) ++count;
    return count;
}

int
getBlockSize(unsigned int flag) {
    int blocksize;
    if (ISFLAG_k(flag))
        return 1024;
    if (ISFLAG_h(flag))
        return 1;

    char *env_blocksize = getenv("BLOCKSIZE");
    if (env_blocksize == NULL)
        return BLOCKSIZE;

    blocksize = atoi(env_blocksize);
    return blocksize == 0 ? BLOCKSIZE : blocksize;
}

char *
getUserNameByUID(uid_t uid) {
    struct passwd *pwd;
    pwd = getpwuid(uid);
    if (pwd == NULL)
        return NULL;
    return pwd->pw_name;
}

char *
getGroupNameByGID(gid_t gid) {
    struct group *grp;
    grp = getgrgid(gid);
    if (grp == NULL)
        return NULL;
    return grp->gr_name;
}

void
getHumanReadable(long long size, char *readable, int len) {
    double num = size;
    char unit = '\0';
    if ((size >> 40) > 0) {
        num = (size >> 30) / 1024.0;
        unit = 'T';
    } else if ((size >> 30) > 0) {
        num = (size >> 20) / 1024.0;
        unit = 'G';
    } else if ((size >> 20) > 0) {
        num = (size >> 10) / 1024.0;
        unit = 'M';
    } else if ((size >> 10) > 0) {
        num /= 1024.0;
        unit = 'K';
    }
    if (num == 0) {
        snprintf(readable, len, "%d%c", (int)num, unit);
    } else if (num < 10) {
        snprintf(readable, len, "%.1f%c", ceil(num * 10) / 10.0, unit);
    } else {
        snprintf(readable, len, "%d%c", (int)ceil(num), unit);
    }
}

int
getHumanReadableLen(long long size) {
    char readable[10];
    getHumanReadable(size, readable, sizeof(readable));
    return strlen(readable);
}

void
print_1(_item_format *format, FTSENT *list, unsigned int flag) {
    FTSENT *cur;
    for (cur = list; cur; cur = cur->fts_link) {
        if (cur->fts_number == NO_PRINT)
            continue;
        if (ISFLAG_i(flag))
            printInode(cur, format);
        if (ISFLAG_s(flag))
            printBlock(cur, format, flag);
        printName(cur, format, flag);
        printf("\n");
    }
}

void
print_l(_item_format *format, FTSENT *list, unsigned int flag) {
    FTSENT *cur;
    if (list->fts_number != NO_PRINT)
        printTotal(format, flag);
    for (cur = list; cur; cur = cur->fts_link) {
        if (cur->fts_number == NO_PRINT)
            continue;
        if (ISFLAG_i(flag))
            printInode(cur, format);
        if (ISFLAG_s(flag))
            printBlock(cur, format, flag);
        if (ISFLAG_l(flag) || ISFLAG_n(flag)) {
            printMode(cur);
            printLinks(cur, format);
            printUser(cur, format, flag);
            printGroup(cur, format, flag);
            printSize(cur, format, flag);
            printTime(cur, flag);
        }
        printName(cur, format, flag);
        if (ISFLAG_1(flag) || ISFLAG_l(flag) || ISFLAG_n(flag)) {
            printf("\n");
        } else {
            printf("  ");
        }
    }
    if (!ISFLAG_l(flag) && !ISFLAG_n(flag) && !ISFLAG_1(flag))
        printf("\n");
}

void
print_C(_item_format *format, FTSENT *list, unsigned int flag) {
    static FTSENT **array;
    static int last = -1;
    FTSENT *p;
    int base, chcnt, col, colwidth, num;
    int numcols, numrows, row;

    colwidth = format->maxlen_name;
    if (ISFLAG_i(flag))
        colwidth += format->maxlen_inode + 1;
    if (ISFLAG_s(flag))
        colwidth += format->maxlen_size + 1;
    if (ISFLAG_F(flag))
        colwidth += 1;
    ++colwidth;

    if (format->termwidth < 2 * colwidth) {
        print_1(format, list, flag);
        return;
    }

    if (format->item_num > last) {
        FTSENT **newarray;
        newarray = realloc(array, format->item_num * sizeof(FTSENT *));
        if (newarray == NULL) {
            print_1(format, list, flag);
            return;
        }
        last = format->item_num;
        array = newarray;
    }
    for (p = list, num = 0; p; p = p->fts_link) {
        if (p->fts_number != NO_PRINT)
            array[num++] = p;
    }

    numcols = format->termwidth / colwidth;
    colwidth = format->termwidth / numcols;
    numrows = num / numcols;
    if (num % numcols)
        ++numrows;

    printTotal(format, flag);

    for (row = 0; row < numrows; ++row) {
        for (base = row, chcnt = col = 0; col < numcols; ++col) {
            if (ISFLAG_i(flag))
                chcnt += printInode(array[base], format);
            if (ISFLAG_s(flag))
                chcnt += printBlock(array[base], format, flag);
            chcnt += printName(array[base], format, flag);
            if ((base += numrows) >= num)
                break;
            while (chcnt++ < colwidth)
                putchar(' ');
        }
        putchar('\n');
    }
}

int
printTotal(_item_format *format, unsigned int flag) {
    int chcnt = 0;
    if (!ISFLAG_l(flag) && !ISFLAG_1(flag))
        return chcnt;
    if (ISFLAG_h(flag)) {
        char readable[5];
        getHumanReadable(format->total_block, readable, sizeof(readable));
        chcnt += printf("total %s\n", readable);
    } else {
        chcnt += printf("total %ld\n", format->total_block);
    }
    return chcnt;
}

int
printBlock(FTSENT *cur, _item_format *format, unsigned int flag) {
    int chcnt = 0;
    int blocksize = getBlockSize(flag);
    int blocknum = (int)ceil(cur->fts_statp->st_blocks * BLOCKSIZE / (double)blocksize);

    if (ISFLAG_h(flag)) {
        char readable[10];
        getHumanReadable(blocknum, readable, sizeof(readable));
        chcnt += printf("%*s ", format->maxlen_blocknum, readable);
    } else {
        chcnt += printf("%*d ", format->maxlen_blocknum, blocknum);
    }
    return chcnt;
}

int
printMode(FTSENT *cur) {
    int chcnt = 0;
    struct stat *st = cur->fts_statp;
    if (st == NULL)
        return chcnt;

    char mode[12];
    strmode(st->st_mode, mode);
    chcnt += printf("%s ", mode);
    return chcnt;
}

int
printLinks(FTSENT *cur, _item_format *format) {
    int chcnt = 0;
    struct stat *st = cur->fts_statp;
    if (st == NULL)
        return chcnt;

    chcnt += printf("%*ld ", format->maxlen_links, st->st_nlink);
    return chcnt;
}

int
printInode(FTSENT *cur, _item_format *format) {
    int chcnt = 0;
    struct stat *st = cur->fts_statp;
    if (st == NULL)
        return chcnt;

    chcnt += printf("%*ld ", format->maxlen_inode, st->st_ino);
    return chcnt;
}

int
printUser(FTSENT *cur, _item_format *format, unsigned int flag) {
    int chcnt = 0;
    char *user;
    struct stat *st = cur->fts_statp;
    if (st == NULL)
        return chcnt;

    if (ISFLAG_n(flag)) {
        chcnt += printf("%*d ", format->maxlen_owner, st->st_uid);
    } else {
        user = getUserNameByUID(st->st_uid);
        if (user != NULL)
            chcnt += printf("%-*s ", format->maxlen_owner, user);
        else
            chcnt += printf("%*d ", format->maxlen_owner, st->st_uid);
    }
    return chcnt;
}

int
printGroup(FTSENT *cur, _item_format *format, unsigned int flag) {
    int chcnt = 0;
    char *group;
    struct stat *st = cur->fts_statp;
    if (st == NULL)
        return chcnt;

    if (ISFLAG_n(flag)) {
        chcnt +=printf("%*d ", format->maxlen_group, st->st_gid);
    } else {
        group = getGroupNameByGID(st->st_gid);
        if (group != NULL)
            chcnt += printf("%-*s ", format->maxlen_group, group);
        else
            chcnt += printf("%*d ", format->maxlen_group, st->st_gid);
    }
    return chcnt;
}

int
printSize(FTSENT *cur, _item_format *format, unsigned int flag) {
    int chcnt = 0;
    struct stat *st = cur->fts_statp;
    if (st == NULL)
        return chcnt;

    long size = st->st_size;

    if (ISFLAG_h(flag)) {
        char readable[10];
        getHumanReadable(size, readable, sizeof(readable));
        chcnt += printf("%*s ", format->maxlen_size, readable);
    } else {
        if (ISFLAG_k(flag)) {
            size = (int)ceil(size / 1024.0);
        }
        chcnt += printf("%*ld ", format->maxlen_size, size);
    }
    return chcnt;
}

int
printTime(FTSENT *cur, unsigned int flag) {
    int chcnt = 0;
    struct stat *st = cur->fts_statp;
    if (st == NULL)
        return chcnt;

    struct tm *tm = localtime(&st->st_mtime);
    if (ISFLAG_c(flag))
        tm = localtime(&st->st_ctime);
    else if (ISFLAG_u(flag))
        tm = localtime(&st->st_atime);

    if (tm == NULL)
        return chcnt;

    char timestr[14];
    if (strftime(timestr, sizeof(timestr), "%b %e %H:%M", tm) == 0)
        return chcnt;

    chcnt += printf("%s ", timestr);
    return chcnt;
}

int
printName(FTSENT *cur, _item_format *format, unsigned int flag) {
    int chcnt = 0;
    char symbol = '\0';
    if (ISFLAG_F(flag)) {
        struct stat *st = cur->fts_statp;
        if (st == NULL)
            return chcnt;

        if (S_ISDIR(st->st_mode)) {
            symbol = '/';
        } else if (S_ISLNK(st->st_mode)) {
            symbol = '@';
        } else if (S_ISSOCK(st->st_mode)) {
            symbol = '=';
        } else if (S_ISFIFO(st->st_mode)) {
            symbol = '|';
        } else if (st->st_mode&(S_IXUSR|S_IXGRP|S_IXOTH)) {
            symbol = '*';
        }
#ifdef S_ISWHT
        else if (S_ISWHT(st->st_mode)) {
            symbol = '%';
        }
#endif
    }
    if (ISFLAG_q(flag)) {
        int i = 0;
        for (; i < strlen(cur->fts_name); ++i) {
            cur->fts_name[i] = isprint(cur->fts_name[i]) ? cur->fts_name[i] : '?';
        }
    }
    if (!ISFLAG_l(flag) && !ISFLAG_1(flag))
        chcnt += printf("%-*s%c  ", format->maxlen_name, cur->fts_name, symbol);
    else
        chcnt += printf("%-s%c", cur->fts_name, symbol);
    return chcnt;
}
