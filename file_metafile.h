#ifndef _FILE_METAFILE_H_
#define _FILE_METAFILE_H_

extern int file_metafile_init(char *file_name);
int file_metafile_find_key(char *key,const int pos_beg, int *pos_cur);
int file_metafile_get_announce_list();

typedef struct _announce{
    char host[100];
    struct _announce *next;
}announce;



#endif
