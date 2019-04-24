#ifndef POLICY_H
#define POLICY_H
#include "peer.h"

#define COMPUTE_RATE_TIME  10

#define UNCHOKE_COUNT  4

#define REQ_SLICE_NUM  5

typedef struct _Unchoke_peers {
    Peer*  unchkpeer[UNCHOKE_COUNT];
    int    count;
    Peer*  optunchkpeer;
} Unchoke_peers;


void init_unchoke_peers();     


int select_unchoke_peer();     
int select_optunchoke_peer();  


int compute_rate();            
int compute_total_rate();      


int is_seed(Peer *node);      


int create_req_slice_msg(Peer *node);  

#endif



