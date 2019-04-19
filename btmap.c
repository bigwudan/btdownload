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




