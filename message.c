#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>



#include "message.h"
#include "file_metafile.h"
#include "peer.h"



int int_to_char(int index,unsigned char *buff)
{
    buff[3] = index%256;
    buff[2] = (index-buff[3])/256%256;
    buff[1] = (index-buff[3]-buff[2]*256)/256/256%256;
    buff[0] = (index-buff[3]-buff[2]*256-buff[1]*256*256)/256/256/256%256;
    return 0;
}

int char_to_int(unsigned int *index, const unsigned char *buff)
{
    *index = buff[3] + buff[2]*256 + buff[1]*256*256+buff[0]*256*256*256;
    return 1;    
}




int http_encode(const unsigned char *dsc, int dsc_len, char *src){
    char asc_char[] = {'0', '1', '2', '3', '4', '5', '6','7', '8', '9',  'A', 'B', 'C', 'D', 'E', 'F' };
    int l = 0;
    for(int i=0; i < dsc_len; i++){
        if(  (dsc[i] >= 48 && dsc[i] <= 57) || (dsc[i] >=65 && dsc[i] <=122)  ){
            src[l++] = dsc[i]; 
        }else{
            char p = 0;
            int t = 0;
            p = dsc[i] >> 4;
            t = p & 0x0F;
            src[l++] = '%';
            src[l++] = asc_char[t];
            t = dsc[i] & 0x0F;
            src[l++] = asc_char[t];
        }
    }
    src[l] = '\0';
    return 1;
}

int message_track(announce *announce_node,unsigned char *buff, unsigned char *hash, unsigned char *peer_id, int port, long uploaded, long downloaded,
        long life, int numwant)
{
    char           encoded_info_hash[100];
    char           encoded_peer_id[100];
    char *url = announce_node->host;
    int key;
    srand(time(NULL));
    key = rand() / 10000;
    http_encode(hash, 20, encoded_info_hash);
    http_encode(peer_id, 20, encoded_peer_id);
    char *p = strstr(url, "http://");
    p = p +7;
    p = strchr(p, '/');
    char *q = url + 7;
    int len = p -q;
    char host[30] = {0};
    memmove(host, q, len+1);
    sprintf(buff, 
            "GET %s&info_hash=%s&peer_id=%s&port=%u"
            "&uploaded=%lld&downloaded=%lld&left=%lld"
            "&event=started&key=%d&compact=1&numwant=%d HTTP/1.0\r\n"
            "Host: %s\r\nUser-Agent: Bittorrent\r\nAccept: */*\r\n"
            "Accept-Encoding: gzip\r\nConnection: closed\r\n\r\n",
            p, encoded_info_hash, encoded_info_hash,port, uploaded, downloaded, life,key,numwant, host 
           );
    return 1;
}


//<pstrlen><pstr><reserved><info_hash><peer_id>
int create_handshake_msg(char *buff, unsigned char *hash, char *peer_id)
{
    char *pstr = "BitTorrent protocol";
    char *p = NULL;
    buff[0] = 19;
    memmove(buff+1, pstr, strlen(pstr));
    p = buff + 1 + strlen(pstr);
    for(int i=0; i < 8; i++){
        p +=i;
        *p = 0;
    }
    p++;
    memmove(p, hash, 20);
    memmove(p+20, peer_id, 20);
    return 1;
}

int create_keep_alive_msg(char *buff)
{
    for(int i =0; i < 4; i++){
        buff[i] = 0;
    }
    return 1;
}


int create_chock_interested_msg(int type,char *buff)
{
    memset(buff, 0, 5);
    buff[3] = 1;
    buff[4] = type;
    return 1;
}

int create_have_msg(int index, char *buff)
{
    memset(buff, 0, 9);
    buff[3] = 5;
    buff[4] = 4;
    char p[4] = {0};
    int_to_char(index,p);
    buff[5] = p[0];
    buff[6] = p[1];
    buff[7] = p[2];
    buff[8] = p[3];
    return 1;
}

int create_bitfield_msg(char *bitfield,int bitfield_len, char *buff)
{
    unsigned char p[4] = {0};
    int_to_char(bitfield_len+1, p);
    memmove(buff, p, 4);
    buff[4] = 5;
    memmove(&buff[5], bitfield, bitfield_len);
    return 1;
}

int create_request_msg(int index,int begin,int length, char *buff)
{
    int i =0;
    buff[3] = 13;
    buff[4] = 6;
    unsigned char c[4] = {0};
    int_to_char(index, c);
    i = 5;
    memmove(&buff[i], c, 4);
    memset(c, 0, 4);
    int_to_char(begin, c );
    i += 4;
    memmove(&buff[i], c, 4);
    memset(c, 0, 4);
    int_to_char(length, c );
    i += 4;
    memmove(&buff[i], c, 4);
    memset(c, 0, 4);
    return 0;
}

int create_piece_msg(int index,int begin,char *block,int b_len,char *buff)
{
    unsigned char c[4] = {0};
    int_to_char(b_len+9, c);
    memmove(buff, c, 4);
    *(buff+4) = 7;
    int_to_char(index, c);
    memmove(buff+5, c, 4);
    int_to_char(begin, c);
    memmove(buff+9, c, 4);
    memmove(buff+13, block, b_len);
    return 0;
}


int create_cancel_msg(int index,int begin,int length,char *buff)
{
    *(buff+3) = 13;
    *(buff+4) = 8;
    unsigned char c[4] = {0};
    int_to_char(index, c);
    memmove(buff+5, c, 4);
    int_to_char(begin, c);
    memmove(buff+9, c, 4);
    int_to_char(length, c);
    memmove(buff+13, c, 4);
    return 0;
}

int create_port_msg(int port,char *buff)
{
    *(buff + 3) = 3;
    *(buff + 4) = 9;
    unsigned char c[4] = {0};
    int_to_char(port, c);
    *(buff + 5) = c[2];
    *(buff+6) = c[3];
    return 0;
}

int is_complete_message(unsigned char *buff,unsigned int len,int *ok_len)
{
    unsigned int   i;
    char           btkeyword[20];

    unsigned char  keep_alive[4]   = { 0x0, 0x0, 0x0, 0x0 };
    unsigned char  chocke[5]       = { 0x0, 0x0, 0x0, 0x1, 0x0};
    unsigned char  unchocke[5]     = { 0x0, 0x0, 0x0, 0x1, 0x1};
    unsigned char  interested[5]   = { 0x0, 0x0, 0x0, 0x1, 0x2};
    unsigned char  uninterested[5] = { 0x0, 0x0, 0x0, 0x1, 0x3};
    unsigned char  have[5]         = { 0x0, 0x0, 0x0, 0x5, 0x4};
    unsigned char  request[5]      = { 0x0, 0x0, 0x0, 0xd, 0x6};
    unsigned char  cancel[5]       = { 0x0, 0x0, 0x0, 0xd, 0x8};
    unsigned char  port[5]         = { 0x0, 0x0, 0x0, 0x3, 0x9};

    if(buff==NULL || len<=0 || ok_len==NULL)  return -1;
    *ok_len = 0;

    btkeyword[0] = 19;
    memcpy(&btkeyword[1],"BitTorrent protocol",19);  // BitTorrentЭ��ؼ���

    unsigned char  c[4];
    unsigned int   length;
    while(*ok_len < len){
        if( memcmp(      &buff[*ok_len], keep_alive, 4    )  == 0   ){
            *ok_len += 4;
            continue;

        }else if(  memcmp(&buff[*ok_len], chocke, 5  ) == 0 ){
            *ok_len +=5;
            continue;
        }else if(memcmp(&buff[*ok_len], unchocke, 5) == 0){
            *ok_len +=5;
            continue;

        }else if( memcmp(&buff[*ok_len], interested, 5) == 0  ){
            *ok_len +=5;
            continue;

        }else if(memcmp(&buff[*ok_len], uninterested, 5) ==0   ){
            *ok_len +=5;
            continue;
        }else if(memcmp(&buff[*ok_len], have, 5) ==0 ){
            *ok_len +=5;
            continue;
        }else if(memcmp(&buff[*ok_len],  request, 5) == 0){
            *ok_len +=5;
            continue;
        }else if(memcmp(&buff[*ok_len], cancel, 5) == 0){
            *ok_len +=5;
            continue;
        }else if(memcmp(&buff[*ok_len], port, 5) == 0){
            *ok_len +=5;
            continue;
        }else{
            length = 0;
            char_to_int(&length, buff+*ok_len);
            //bt
            if(buff[*ok_len+4] == 5 ||  buff[*ok_len + 4] == 7){
                if( (len - *ok_len) < length +4  ){
                    return 0;
                }else{
                    *ok_len += 4 + length;
                }
            }else{
                return 0;
            }
        }
    }
    return 0;
}

int process_handshake_msg(Peer *peer,unsigned char *buff,int len)
{

    if(peer == NULL || buff == NULL) return -1;
    if(memcmp(buff+28, info_hash, 20)){
        peer->state = CLOSING;
        return -1;
    }
    memcpy(peer->id, buff+48, 20);
    *(peer->id+20) = '\0';
    if(peer->state == INITIAL){
        peer->state = HALFSHAKED;
        create_handshake_msg(peer->in_buff + peer->buff_len,info_hash, peer->id  );
    }
    if(peer->state == HALFSHAKED){
        peer->state = HANDSHAKED;
    }
    peer->start_timestamp = time(NULL);
    return 0;
}


void discard_send_buffer(Peer *peer)
{
    struct linger  lin;
    int            lin_len;
    lin.l_onoff  = 1;
    lin.l_linger = 0;
    lin_len      = sizeof(lin);
    if(peer->socket > 0) {
        setsockopt(peer->socket,SOL_SOCKET,SO_LINGER,(char *)&lin,lin_len);
    }
}























