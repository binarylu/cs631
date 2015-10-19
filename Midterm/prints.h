#ifndef __PRINTS_H__
#define __PRINTS_H__

#include <stdlib.h>
#include <fts.h>
#include <time.h>
#include <sys/stat.h>
#include <bsd/string.h>
#include <pwd.h>
#include <grp.h>
#include <math.h>
#include <ctype.h>

#include "options.h"

#define BLOCKSIZE 512

typedef struct {
    int maxlen_links;
    int maxlen_owner;
    int maxlen_group;
    int maxlen_size;
    int maxlen_blocknum;
    int maxlen_inode;
    int maxlen_name;
    long total_block;
    int item_num;
    int termwidth;
} _item_format;

int getNumLen(long long num);
int getBlockSize(unsigned int flag);
char *getUserNameByUID(uid_t uid);
char *getGroupNameByGID(uid_t uid);
void getHumanReadable(long long size, char *readable, int len);
int getHumanReadableLen(long long size);

void print_1(_item_format *format, FTSENT *list, unsigned int flag);
void print_l(_item_format *format, FTSENT *list, unsigned int flag);
void print_C(_item_format *format, FTSENT *list, unsigned int flag);
int printTotal(_item_format *format, unsigned int flag);
int printBlock(FTSENT *cur, _item_format *format, unsigned int flag);
int printMode(FTSENT *cur);
int printLinks(FTSENT *cur, _item_format *format);
int printInode(FTSENT *cur, _item_format *format);
int printUser(FTSENT *cur, _item_format *format, unsigned int flag);
int printGroup(FTSENT *cur, _item_format *format, unsigned int flag);
int printSize(FTSENT *cur, _item_format *format, unsigned int flag);
int printTime(FTSENT *cur, unsigned int flag);
int printName(FTSENT *cur, _item_format *format, unsigned int flag);

#endif /* __PRINTS_H__ */
