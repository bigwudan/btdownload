#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

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

int get_files_count()
{
    int count = 0;
    filedowninfo *head = NULL;
    head = filedowninfo_head;
    while(head){
        count++;
        head = head->next;
    }
    return count;
}

int create_files()
{
    if(chdir("test") == 0){
        mkdir("test", 0777);
        chdir("test");
    }
        
    char buff[1] = { 0x0 };


    filedowninfo *head = filedowninfo_head;
    fds = calloc(get_files_count(), sizeof(int));
    int i =0;

    while(head){
            
        fds[i] = open(head->file_name, O_RDWR|O_CREAT,0777);

        lseek(fds[i], head->file_len-1, SEEK_SET);

        write(fds[i],buff,1);
        
        head = head->next;

    }
    return 1;
}

int write_btcache_node_to_harddisk(Btcache *node)
{

    long long     line_position;
    filedowninfo         *p;
    int           i;

    if(node == NULL) return 1;
    line_position = node->index*piece_length + node->begin;
    int file_count = get_files_count(); 
    if(file_count == 1){
        lseek(fds[0], line_position, SEEK_SET);
        write(fds[0], node->buff, node->length);
        return 0;
    }

    p = filedowninfo_head;

    while(1){
        if(line_position < p->file_len && line_position + node->length < p->file_len ){
            lseek(p->fd, line_position, SEEK_SET);
            write(p->fd, node->buff, node->length);
            break;
        }else if( line_position < p->file_len && line_position + node->length > p->file_len ){
            int left = 0;
            int offset = 0;
            offset =    p->file_len - line_position;
            left = node->length - offset;
            lseek(p->fd, line_position, SEEK_SET);
            write(p->fd, node->buff+node->begin, offset);
            i++;
            p = p->next;
            while(left <= 0 ){
                if( left < p->file_len  && left + node->length > p->file_len   ){
                    write(p->fd, node->buff + node->begin + offset, offset);
                    left = 0;
                    break;
                }else{
                    write(p->fd, node->buff + node->begin + offset, offset);
                    left = left - p->file_len;
                    offset += p->file_len;
                }
            }
        }
    
        i++;

    


    


    
    
    }

}









