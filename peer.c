#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#include "btmap.h"
#include "file_metafile.h"
#include "message.h"
#include "peer.h"

Peer *peer_head = NULL;

int   
initialize_peer (Peer *peer)
{
    peer = calloc(sizeof(Peer), 1);
    peer->state = INITIAL;
    peer->am_choking = 1;
    peer->peer_choking = 1;
    peer->in_buff = calloc(MSG_SIZE,1 );
    peer->out_msg = calloc(MSG_SIZE, 1);
    peer->out_msg_copy = calloc(MSG_SIZE, 1);
    return 1;
}        

Peer* add_peer_node()
{
    Peer *peer = NULL;
    initialize_peer(peer);
    if(peer_head == NULL){
        peer_head = peer;
    }else{
        Peer *p =  peer_head;
        while(p->next){
            p = p->next;
        }
        p->next = peer;
    }
    return peer;
} 

int del_peer_node(Peer *peer)
{
    if(peer == peer_head){
        free(peer);peer = NULL;
        peer_head = NULL;
        return 0;
    }else{
        Peer *p = peer_head;
        Peer *q = NULL;
        while(p){
            if(p == peer){
                q->next = p->next;
                free(p);
                p = NULL;
                return 0;
            }
            q = p;
            p = p->next;
        }
    }
    return 0; 
}

int cancel_request_list(Peer *node)
{
    if(node->Request_piece_head == NULL) return 1;
    Request_piece *p = NULL;
    while(node->Request_piece_head){
        p = node->Request_piece_head;
        node->Request_piece_head = node->Request_piece_head->next;
        free(p);
        p= NULL;
    }
    return 1;
}





int cancel_requested_list(Peer *node)
{
    if(node->Requested_piece_head == NULL) return 1;
    Request_piece *p = NULL;
    while(node->Requested_piece_head){
        p = node->Requested_piece_head;
        node->Requested_piece_head = node->Requested_piece_head->next;
        free(p);
        p= NULL;
    }
    return 1;
}


void  free_peer_node(Peer *node)
{
    if(node->in_buff){
        free(node->in_buff);
        node->in_buff = NULL;
    }
    if(node->out_msg){
        free(node->out_msg);
        node->out_msg = NULL;
    }
    if(node->out_msg_copy){
        free(node->out_msg_copy);
        node->out_msg_copy = NULL;
    }
    cancel_request_list(node);
    cancel_requested_list(node);
    free_peer_node(node);
    return ;
}

void  release_memory_in_peer()
{
    Peer *p;
    if(peer_head == NULL)  return;
    p = peer_head;
    while(p != NULL) {
        peer_head = peer_head->next;
        free_peer_node(p);
        p = peer_head;
    }
}










