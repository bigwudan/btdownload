#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "message.h"
#include "file_metafile.h"
#include "peer.h"

#define btcache_len 1024



Btcache *btcache_head = NULL;
Btcache *last_piece = NULL;

int *fds = NULL;

int last_piece_count = 0;
int last_slice_len = 0;
int last_piece_index = 0;

int have_piece_index[64] = {0};


Btcache* initialize_btcache_node()
{
    Btcache *node;

    node = (Btcache *)malloc(sizeof(Btcache));
    if(node == NULL) {
        printf("%s:%d malloc error\n",__FILE__,__LINE__);
        return NULL;
    }

    node->buff = (unsigned char *)malloc(16*1024);
    if(node->buff == NULL) {
        if(node != NULL)  free(node);
        printf("%s:%d malloc error\n",__FILE__,__LINE__);
        return NULL;
    }

    node->index  = -1;
    node->begin  = -1;
    node->length = -1;

    node->in_use       =  0;
    node->read_write   = -1;
    node->is_full      =  0;
    node->is_writed    =  0;
    node->access_count =  0;
    node->next         =  NULL;
    return node;
}

void release_last_piece()
{
    Btcache *p = last_piece;
    while(p != NULL) {
        last_piece = p->next;
        if(p->buff != NULL) free(p->buff);
        free(p);
        p = last_piece;
    }
}



void release_memory_in_btcache()
{
    Btcache *p;

    p = btcache_head;
    while(p != NULL) {
        btcache_head = p->next;
        if(p->buff != NULL) free(p->buff);
        free(p);
        p = btcache_head;
    }

    release_last_piece();
    if(fds != NULL)  free(fds);
}


int create_btcache()
{
    int     i;
    Btcache *node, *last;
    for(i =0; i < btcache_len; i++){
        node = initialize_btcache_node();
        if(node == NULL){
            release_memory_in_btcache();
            return -1;
        }
        if(btcache_head == NULL){
            btcache_head = node;
            last_piece = node;
        }else{
            last_piece->next = node;
            last_piece = node;
        }
    }


    int count = file_size % piece_length / (16*1024);
    if(file_size % piece_length % (16*1024) != 0)  count++;
    last_piece_count = count;

    last_slice_len = file_size % piece_length % (16*1024);
    if(last_slice_len == 0)  last_slice_len = 16*1024;

    last_piece_index = pieces_length / 20 -1;

    
    while(count){
        node = initialize_btcache_node();
        if(node == NULL){
            release_memory_in_btcache();
            return -1;
        }
        if(btcache_head == NULL){
            btcache_head = node;
            last = btcache_head;
        }else{
            last->next = node;
            last = node;
        }
        count--;
    }
    for(i = 0; i < 64; i++) {
        have_piece_index[i] = -1;
    }
}





