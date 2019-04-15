#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>


#include "file_metafile.h"

//文件内容指针
unsigned char *file_content = NULL;
//文件尺寸
long long file_size = 0;
//announce 头文件
announce *announce_head = NULL;


//读取文件
int
file_metafile_init(char *file_name)
{
    if(!file_name) return -1;
    FILE *file = NULL;
    file = fopen(file_name, "rb");
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    printf("len11=%lld\n", file_size);
    file_content = calloc(file_size, 1);
    if(!file_content) return -1;
    fseek(file, 0, SEEK_SET);
    for(long long i = 0; i < file_size ; i++){
        file_content[i] = fgetc(file);
    }
    file_metafile_get_announce_list();
    return 1;
}

//查找key
int
file_metafile_find_key(char *key,const int pos_beg, int *pos_cur)
{
    for(int n = pos_beg; n < file_size; n++){
       if( memcmp(file_content+n, key, strlen(key)) == 0 ) {
           *pos_cur = n; 
           return 0;
       
       }   
    }
    return -1;
}
//得到bt基本信息
int
file_metafile_get_announce_list()
{
    int pos_cur = 0;
    int flag = file_metafile_find_key("d8:announce", 0, &pos_cur);
    pos_cur += 11;
    int len = 0;
    announce_head = calloc(sizeof(announce), 1);
    for(int i = pos_cur; i < file_size; i++){
        if(file_content[i] >= 48 && file_content[i] <= 57 ){
            len = len*10 + file_content[i] - '0';
        }else{
            i++;
            memmove(announce_head->host, &file_content[i], len);
            pos_cur = i +len; 
            break;
        }
    }
    
    int beg = pos_cur;
    //announc_list
    file_metafile_find_key("13:announce-list", beg, &pos_cur);
    pos_cur += 16;
    //skip l
    pos_cur++;
    for(int i = pos_cur; i < file_size; i++){
        //list beg
        if(file_content[i] == 'l'){
            i++;
            len = 0;
            while( file_content[i] >= 48 && file_content[i] <= 57 ){
                len = len*10+file_content[i] - '0';
                i++;
            }
            if(file_content[i] == ':'){
                //skip :
                i++;
                announce * announce_list = calloc(sizeof(announce), 1);
                memmove(announce_list->host, &file_content[i], len);
                announce *p = announce_head;
                while(p->next) p = p->next;
                p->next = announce_list;
                //skip e
                i = i+len ;
            }else{
                exit(1);
            }
        }else if(file_content[i] == 'e'){
            break;
        }

    }

    return 1;
}






