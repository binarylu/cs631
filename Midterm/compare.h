#ifndef __COMPARE_H__
#define __COMPARE_H__

#include <sys/stat.h>
#include <fts.h>
#include <string.h>

int sortByName(const FTSENT *a, const FTSENT *b);
int rsortByName(const FTSENT *a, const FTSENT *b);

int sortBySize(const FTSENT *a, const FTSENT *b);
int rsortBySize(const FTSENT *a, const FTSENT *b);

int sortByMtime(const FTSENT *a, const FTSENT *b);
int rsortByMtime(const FTSENT *a, const FTSENT *b);

int sortByAtime(const FTSENT *a, const FTSENT *b);
int rsortByAtime(const FTSENT *a, const FTSENT *b);

int sortByCtime(const FTSENT *a, const FTSENT *b);
int rsortByCtime(const FTSENT *a, const FTSENT *b);

#endif /* __COMPARE_H__ */
