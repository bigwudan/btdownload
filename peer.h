#ifndef _PEER_H_
#define _PEER_H_

#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "btmap.h"
#include "file_metafile.h"
#include "message.h"


#define  INITIAL            -1      // 表明处于初始化状态
#define  HALFSHAKED         0   // 表明处于半握手状态
#define  HANDSHAKED         1   // 表明处于全握手状态
#define  SENDBITFIELD       2   // 表明处于已发送位图状态
#define  RECVBITFIELD       3   // 表明处于已接收位图状态
#define  DATA               4   // 表明处于与peer交换数据的状态
#define  CLOSING            5   // 表明处于即将与peer断开的状态

// 发送和接收缓冲区的大小,16K可以存放一个slice,2K用来存放其他消息
 #define  MSG_SIZE  (2*1024+16*1024)

typedef struct _Request_piece {
    int     index;                
    int     begin;                
    int     length;               
    struct _Request_piece *next;
} Request_piece;

typedef struct  _Peer {
    int            socket;                
    char           ip[16];                
    unsigned short port;                  
    char           id[21];                

    int            state;                 

    int            am_choking;            
    int            am_interested;         
    int            peer_choking;          
    int            peer_interested;       

    unsigned char  *bitmap;                

    char           *in_buff;              
    int            buff_len;              
    char           *out_msg;              
    int            msg_len;               
    char           *out_msg_copy;         
    int            msg_copy_len;          
    int            msg_copy_index;        

    Request_piece  *Request_piece_head;   
    Request_piece  *Requested_piece_head; 

    unsigned int   down_total;            
    unsigned int   up_total;              

    time_t         start_timestamp;       
    time_t         recet_timestamp;       

    time_t         last_down_timestamp;   
    time_t         last_up_timestamp;     
    long long      down_count;            
    long long      up_count;              
    float          down_rate;             
    float          up_rate;               

    struct _Peer   *next;                 
} Peer;
extern Peer *peer_head ;
extern int  initialize_peer (Peer *peer);
extern Peer* add_peer_node();
extern int del_peer_node(Peer *peer);
int cancel_request_list(Peer *node);
int cancel_requested_list(Peer *node);
void free_peer_node(Peer *node);
void  release_memory_in_peer();
int process_unchoke_msg(Peer *peer,unsigned char *buff,int len);


#endif
