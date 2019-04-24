#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "file_metafile.h"
#include "peer.h"
#include "data.h"
#include "message.h"
#include "policy.h"

Unchoke_peers  unchoke_peers;
long long      total_down = 0L, total_up = 0L;
float          total_down_rate = 0.0F, total_up_rate = 0.0F;
int            total_peers = 0;

extern int     end_mode;
extern unsigned char  *btmap_list;
extern Peer    *peer_head;
extern int     pieces_length;
extern int     piece_length;

extern Btcache *btcache_head;
extern int     last_piece_index;
extern int     last_piece_count;
extern int     last_slice_len;
extern int     download_piece_num;



void init_unchoke_peers()
{
    int i;

    for(i = 0; i < UNCHOKE_COUNT; i++) {
        *(unchoke_peers.unchkpeer + i) = NULL;      
    }

    unchoke_peers.count = 0;
    unchoke_peers.optunchkpeer = NULL;
}

int is_in_unchoke_peers(Peer *node)
{
    int i;

    for(i = 0; i < unchoke_peers.count; i++) {
        if( node == (unchoke_peers.unchkpeer)[i] )  return 1;
    }

    return 0;
}



int get_last_index(Peer **array,int len)
{
    int i, j = -1;

    if(len <= 0) return j;
    else j = 0;

    for(i = 0; i < len; i++)
        if( array[i]->down_rate < array[j]->down_rate )  j = i;

    return j;
}

int select_unchoke_peer()
{
    Peer*  p;
    Peer*  now_fast[UNCHOKE_COUNT];
    Peer*  force_choke[UNCHOKE_COUNT];
    int    unchoke_socket[UNCHOKE_COUNT], choke_socket[UNCHOKE_COUNT];
    int    i, j, index = 0, len = UNCHOKE_COUNT;

    for(i = 0; i < len; i++) {
        now_fast[i]       = NULL;
        force_choke[i]    = NULL;
        unchoke_socket[i] = -1;
        choke_socket[i]   = -1;
    }


    for(i = 0, j = 0; i < unchoke_peers.count; i++) {
        p = peer_head;
        while(p != NULL) {
            if(p == unchoke_peers.unchkpeer[i])  break;
            p = p->next;
        }
        if(p == NULL)  { unchoke_peers.unchkpeer[i] = NULL; j++; }
    }
    if(j != 0) {
        unchoke_peers.count = unchoke_peers.count - j;
        for(i = 0, j = 0; i < len; i++) {
            if(unchoke_peers.unchkpeer[i] != NULL) {
                force_choke[j] = unchoke_peers.unchkpeer[i];
                j++;
            }
        }
        for(i = 0; i < len; i++) {
            unchoke_peers.unchkpeer[i] = force_choke[i];
            force_choke[i] = NULL;
        }
    }

    for(i = 0, j = -1; i < unchoke_peers.count; i++) {
        if( (unchoke_peers.unchkpeer)[i]->up_rate > 50*1024 &&
                (unchoke_peers.unchkpeer)[i]->down_rate < 0.1*1024 ) {
            j++;
            force_choke[j] = unchoke_peers.unchkpeer[i];
        }
    }


    p = peer_head;
    while(p != NULL) {
        if(p->state==DATA && is_interested(bitmap,&(p->bitmap)) && is_seed(p)!=1) {

            for(i = 0; i < len; i++) {
                if(p == force_choke[i]) break;
            }
            if(i == len) {
                if( index < UNCHOKE_COUNT ) {
                    now_fast[index] = p; 
                    index++; 
                } else {
                    j = get_last_index(now_fast,UNCHOKE_COUNT);
                    if(p->down_rate >= now_fast[j]->down_rate) now_fast[j] = p;
                }
            }
        }
        p = p->next;
    }


    for(i = 0; i < index; i++) {
        Peer*  q = now_fast[i];
        unchoke_socket[i] = q->socket;
    }


    for(i = 0; i < unchoke_peers.count; i++) {
        Peer*  q = (unchoke_peers.unchkpeer)[i];
        choke_socket[i] = q->socket;
    }

    for(i = 0; i < index; i++) {
        if( is_in_unchoke_peers(now_fast[i]) == 1) {
            for(j = 0; j < len; j++) {
                Peer*  q = now_fast[i];
                if(q->socket == unchoke_socket[i])  unchoke_socket[i] = -1;
                if(q->socket == choke_socket[i])    choke_socket[i]   = -1;
            }
        }
    }


    for(i = 0; i < index; i++) {
        (unchoke_peers.unchkpeer)[i] = now_fast[i];
    }
    unchoke_peers.count = index;


    p = peer_head;
    while(p != NULL) {
        for(i = 0; i < len; i++) {
            if(unchoke_socket[i]==p->socket && unchoke_socket[i]!=-1) {
                p->am_choking = 0;
                create_chock_interested_msg(1,p);
            }
            if(choke_socket[i]==p->socket && unchoke_socket[i]!=-1) {
                p->am_choking = 1;
                cancel_requested_list(p);
                create_chock_interested_msg(0,p);
            }
        }
        p = p->next;
    }


    return 0;
}

int *rand_num = NULL;
int get_rand_numbers(int length)
{
    int i, index, piece_count, *temp_num;

    if(length == 0)  return -1;
    piece_count = length;

    rand_num = (int *)malloc(piece_count * sizeof(int));
    if(rand_num == NULL)    return -1;

    temp_num = (int *)malloc(piece_count * sizeof(int));
    if(temp_num == NULL)    return -1;
    for(i = 0; i < piece_count; i++)  temp_num[i] = i;

    srand(time(NULL));
    for(i = 0; i < piece_count; i++) {
        index = (int)( (float)(piece_count-i) * rand() / (RAND_MAX+1.0) );
        rand_num[i] = temp_num[index];
        temp_num[index] = temp_num[piece_count-1-i];
    }

    if(temp_num != NULL)  free(temp_num);
    return 0;
}

int select_optunchoke_peer()
{
    int   count = 0, index, i = 0, j, ret;
    Peer  *p = peer_head; 


    while(p != NULL) {
        count++;
        p =  p->next;
    }


    if(count <= UNCHOKE_COUNT)  return 0;

    ret = get_rand_numbers(count);
    if(ret < 0) {
        printf("%s:%d get rand numbers error\n",__FILE__,__LINE__);
        return -1;
    }
    while(i < count) {

        index = rand_num[i];

        p = peer_head;
        j = 0;
        while(j < index && p != NULL) {
            p = p->next;
            j++;
        }

        if( is_in_unchoke_peers(p) != 1 && is_seed(p) != 1 && p->state == DATA &&
                p != unchoke_peers.optunchkpeer && is_interested(bitmap,&(p->bitmap)) ) {

            if( (unchoke_peers.optunchkpeer) != NULL ) {
                Peer  *temp = peer_head;
                while( temp != NULL ) {
                    if(temp == unchoke_peers.optunchkpeer) break;
                    temp = temp->next;
                }
                if(temp != NULL) {
                    (unchoke_peers.optunchkpeer)->am_choking = 1;
                    create_chock_interested_msg(0,unchoke_peers.optunchkpeer);
                }
            }

            p->am_choking = 0;
            create_chock_interested_msg(1,p);
            unchoke_peers.optunchkpeer = p;
            break;
        }

        i++;
    }

    if(rand_num != NULL) { free(rand_num); rand_num = NULL; }
    return 0;
}


int compute_rate()
{
    Peer    *p       = peer_head;
    time_t  time_now = time(NULL);
    long    t        = 0;

    while(p != NULL) {
        if(p->last_down_timestamp == 0) {
            p->down_rate  = 0.0f;
            p->down_count = 0;
        } else {
            t = time_now - p->last_down_timestamp;
            if(t == 0)  printf("%s:%d time is 0\n",__FILE__,__LINE__);
            else  p->down_rate = p->down_count / t;
            p->down_count          = 0;
            p->last_down_timestamp = 0;
        }

        if(p->last_up_timestamp == 0) {
            p->up_rate  = 0.0f;
            p->up_count = 0;
        } else {
            t = time_now - p->last_up_timestamp;
            if(t == 0)  printf("%s:%d time is 0\n",__FILE__,__LINE__);
            else  p->up_rate = p->up_count / t;
            p->up_count          = 0;
            p->last_up_timestamp = 0;
        }

        p = p->next;
    }

    return 0;
}

int compute_total_rate()
{
    Peer *p = peer_head;

    total_peers     = 0;
    total_down      = 0;
    total_up        = 0;  
    total_down_rate = 0.0f;
    total_up_rate   = 0.0f;

    while(p != NULL) {
        total_down      += p->down_total;
        total_up        += p->up_total;
        total_down_rate += p->down_rate;
        total_up_rate   += p->up_rate;

        total_peers++;
        p = p->next;
    }

    return 0;
}



int is_seed(Peer *node)
{
    int            i;
    unsigned char  c = (unsigned char)0xFF, last_byte;
    unsigned char  cnst[8] = { 255, 254, 252, 248, 240, 224, 192, 128 };

    if(node->bitmap.bitfield == NULL)  return 0;

    for(i = 0; i < node->bitmap.bitfield_length-1; i++) {
        if( (node->bitmap.bitfield)[i] != c ) return 0;
    }


    last_byte = node->bitmap.bitfield[i];

    i = 8 * node->bitmap.bitfield_length - node->bitmap.valid_length; 

    if(last_byte >= cnst[i]) return 1;
    else return 0;
}

int create_req_slice_msg(Peer *node)
{
    int index, begin, length = 16*1024;
    int i, count = 0;

    if(node == NULL)  return -1;

    if(node->peer_choking==1 || node->am_interested==0 )  return -1;



    Request_piece *p = node->Request_piece_head, *q = NULL;
    if(p != NULL) {
        while(p->next != NULL)  { p = p->next; } 


        int last_begin = piece_length - 16*1024;

        if(p->index == last_piece_index) {
            last_begin = (last_piece_count - 1) * 16 * 1024;
        }


        if(p->begin < last_begin) {
            index = p->index;
            begin = p->begin + 16*1024;
            count = 0;

            while(begin!=piece_length && count<1) {

                if(p->index == last_piece_index) {
                    if( begin == (last_piece_count - 1) * 16 * 1024 )
                        length = last_slice_len;
                }

                create_request_msg(index,begin,length,node);

                q = (Request_piece *)malloc(sizeof(Request_piece));
                if(q == NULL) { 
                    printf("%s:%d error\n",__FILE__,__LINE__);
                    return -1;
                }
                q->index  = index;
                q->begin  = begin;
                q->length = length;
                q->next   = NULL;
                p->next   = q;
                p         = q;

                begin += 16*1024;
                count++;
            }

            return 0;  
        }
    }

    if(get_rand_numbers(pieces_length/20) == -1) {
        printf("%s:%d error\n",__FILE__,__LINE__);
        return -1;
    }

    for(i = 0; i < pieces_length/20; i++) {
        index = rand_num[i];


        if( get_bit_value(&(node->bitmap),index) != 1)  continue;

        if( get_bit_value(bitmap,index) == 1) continue;


        Peer          *peer_ptr = peer_head;
        Request_piece *reqt_ptr;
        int           find = 0;
        while(peer_ptr != NULL) {
            reqt_ptr = peer_ptr->Request_piece_head;
            while(reqt_ptr != NULL) {
                if(reqt_ptr->index == index)  { find = 1; break; }
                reqt_ptr = reqt_ptr->next;
            }
            if(find == 1) break;

            peer_ptr = peer_ptr->next;
        }
        if(find == 1) continue;

        break; 
    }
    if(i == pieces_length/20) {
        if(end_mode == 0)  end_mode = 1;
        for(i = 0; i < pieces_length/20; i++) {
            if( get_bit_value(bitmap,i) == 0 )  { index = i; break; }
        }

        if(i == pieces_length/20) {
            printf("Can not find an index to IP:%s\n",node->ip);
            return -1;
        }
    }


    begin = 0;
    count = 0;
    p = node->Request_piece_head;
    if(p != NULL)
        while(p->next != NULL)  p = p->next;
    while(count < 4) {

        if(index == last_piece_index) {
            if(count+1 > last_piece_count) 
                break;
            if(begin == (last_piece_count - 1) * 16 * 1024)
                length = last_slice_len;
        }

        create_request_msg(index,begin,length,node);

        q = (Request_piece *)malloc(sizeof(Request_piece));
        if(q == NULL) { printf("%s:%d error\n",__FILE__,__LINE__); return -1; }
        q->index  = index;
        q->begin  = begin;
        q->length = length;
        q->next   = NULL;
        if(node->Request_piece_head == NULL)  { node->Request_piece_head = q; p = q; }
        else  { p->next = q; p = q; }
        begin += 16*1024;
        count++;
    }

    if(rand_num != NULL)  { free(rand_num); rand_num = NULL; }
    return 0;
}





int create_req_slice_msg_from_btcache(Peer *node)
{
    Btcache        *b = btcache_head, *b_piece_first;
    Peer           *p;
    Request_piece  *r;
    int            slice_count = piece_length / (16*1024);
    int            count = 0, num, valid_count;
    int            index = -1, length = 16*1024;

    while(b != NULL) {
        if(count%slice_count == 0) {
            num           = slice_count;
            b_piece_first = b;
            valid_count   = 0;
            index         = -1;


            while(num>0 && b!=NULL) {
                if(b->in_use==1 && b->read_write==1 && b->is_writed==0)
                    valid_count++;
                if(index==-1 && b->index!=-1) index = b->index;
                num--;
                count++;
                b = b->next;
            }


            if(valid_count>0 && valid_count<slice_count) {

                p = peer_head;
                while(p != NULL) {
                    r = p->Request_piece_head;
                    while(r != NULL) {
                        if(r->index==index && index!=-1) break;
                        r = r->next;
                    }
                    if(r != NULL) break;
                    p = p->next;
                }

                if(p==NULL && get_bit_value(&(node->bitmap),index)==1) {
                    int request_count = 5;
                    num = 0;

                    r = node->Request_piece_head;
                    if(r != NULL) {
                        while(r->next != NULL) r = r->next;
                    }
                    while(num<slice_count && request_count>0) {
                        if(b_piece_first->in_use == 0) {
                            create_request_msg(index,num*length,length,node);

                            Request_piece *q;
                            q = (Request_piece *)malloc(sizeof(Request_piece));
                            if(q == NULL) { 
                                printf("%s:%d error\n",__FILE__,__LINE__);
                                return -1;
                            }
                            q->index  = index;
                            q->begin  = num*length;
                            q->length = length;
                            q->next   = NULL;
                            printf("create request from btcache index:%-6d begin:%-6x\n",
                                    index,q->begin);
                            if(r == NULL) {
                                node->Request_piece_head = q;
                                r = q;
                            } else{
                                r->next = q;
                                r = q;
                            }
                            request_count--;
                        }
                        num++;
                        b_piece_first = b_piece_first->next;
                    }
                    return 0;
                }
            }
        }
    }

    return -1;
}
















