#ifndef _FILE_METAFILE_H_
#define _FILE_METAFILE_H_

extern int file_metafile_init(char *file_name);
int file_metafile_find_key(char *key,const int pos_beg, int *pos_cur);
int file_metafile_get_announce_list();

int file_metafile_get_files_info();
typedef struct _announce{
    char host[100];
    struct _announce *next;
}announce;

typedef struct _file_down_info{
	char file_name[60];
	long file_len;
	int fd;
	struct _file_down_info *next;


}filedowninfo;

typedef struct _piece{
	char hash[20];

}piece;


int get_info_hash();




#endif
