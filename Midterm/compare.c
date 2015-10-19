#include "compare.h"

int
sortByName(const FTSENT *a, const FTSENT *b) {
    return (strcoll(a->fts_name, b->fts_name));
}

int
rsortByName(const FTSENT *a, const FTSENT *b) {
    return -sortByName(a, b);
}

int
sortBySize(const FTSENT *a, const FTSENT *b) {
    if (a->fts_statp->st_size < b->fts_statp->st_size)
        return 1;
    if (a->fts_statp->st_size > b->fts_statp->st_size)
        return -1;
    return sortByName(a, b);
}

int
rsortBySize(const FTSENT *a, const FTSENT *b) {
    return -sortBySize(a, b);
}

int
sortByMtime(const FTSENT *a, const FTSENT *b) {
    if (a->fts_statp->st_mtime < b->fts_statp->st_mtime)
        return 1;
    if (a->fts_statp->st_mtime > b->fts_statp->st_mtime)
        return -1;
    return sortByName(a, b);
}

int
rsortByMtime(const FTSENT *a, const FTSENT *b) {
    return -sortByMtime(a, b);
}

int
sortByAtime(const FTSENT *a, const FTSENT *b) {
    if (a->fts_statp->st_atime < b->fts_statp->st_atime)
        return 1;
    if (a->fts_statp->st_atime > b->fts_statp->st_atime)
        return -1;
    return sortByName(a, b);
}

int
rsortByAtime(const FTSENT *a, const FTSENT *b) {
    return -sortByAtime(a, b);
}

int
sortByCtime(const FTSENT *a, const FTSENT *b) {
    if (a->fts_statp->st_ctime < b->fts_statp->st_ctime)
        return 1;
    if (a->fts_statp->st_ctime > b->fts_statp->st_ctime)
        return -1;
    return sortByName(a, b);
}

int
rsortByCtime(const FTSENT *a, const FTSENT *b) {
    return -sortByCtime(a, b);
}
