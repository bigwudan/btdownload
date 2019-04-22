#ifndef DATA_H
#define DATA_H
#include "peer.h"
typedef struct _Btcache {
    unsigned char   *buff;        
    int             index;        
    int             begin;        
    int             length;       
    unsigned char   in_use;       
    unsigned char   read_write;   
    unsigned char   is_full;      
    unsigned char   is_writed;    
    int             access_count; 
    struct _Btcache *next;
} Btcache;

Btcache* initialize_btcache_node();  
int  create_btcache();               
void release_memory_in_btcache();    

int get_files_count();               
int create_files();                  


int write_btcache_node_to_harddisk(Btcache *node);


int read_slice_from_harddisk(Btcache *node);

int write_piece_to_harddisk(int sequence,Peer *peer);

int read_piece_from_harddisk(Btcache *p, int index);


int write_btcache_to_harddisk(Peer *peer);

int release_read_btcache_node(int base_count);

void clear_btcache_before_peer_close(Peer *peer);


int write_slice_to_btcache(int index,int begin,int length,
        unsigned char *buff,int len,Peer *peer);

int read_slice_for_send(int index,int begin,int length,Peer *peer);


int write_last_piece_to_btcache(Peer *peer);
int write_slice_to_last_piece(int index,int begin,int length,
        unsigned char *buff,int len,Peer *peer);
int read_last_piece_from_harddisk(Btcache *p, int index);
int read_slice_for_send_last_piece(int index,int begin,int length,Peer *peer);
void release_last_piece();

#endif


