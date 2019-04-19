#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "btmap.h"
#include "file_metafile.h"



unsigned char *btmap_list = NULL;

unsigned int bitmap_len = 0;
unsigned int bitmap_valid_len = 0;



int btmap_init()
{
    int len = piece_count/8;
    bitmap_len = len;
    if(piece_count%8) len++;
    btmap_list = calloc(sizeof(char),  len);
    return 1;
}

int btmap_set(int val)
{
    int count = val/8;
    unsigned char pos = val%8;
    unsigned char p =  1 << (7-pos)  ;
    btmap_list[count] = btmap_list[count] | p;
    return 1;
}

int btmap_get_val(int val){
    int count = val/8;
    unsigned char pos = val%8;
    unsigned char p =  1 << (7-pos)  ;
    if( p & btmap_list[count] ){
        return 1;
    }else{
        return 0;
    } 
}

int is_interested(char *dst,char *src)
{
    char t_dst = 0;
    char t_src= 0;
    char p[] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80};
    for(int i =0; i < bitmap_len; i++){
        t_dst = dst[i];
        t_src = src[i];
        for(int j =0; j < 8; j++   ){
            if(  ((t_dst & p[j]) > 0 ) && ((t_src & p[j]) == 0)    ){
                return 1;
            }
        }
    }
    return 0;
}




