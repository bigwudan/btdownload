#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "message.h"
#include "file_metafile.h"



int int_to_char(int index,unsigned char *buff)
{
    c[3] = i%256;
    c[2] = (i-c[3])/256%256;
    c[1] = (i-c[3]-c[2]*256)/256/256%256;
    c[0] = (i-c[3]-c[2]*256-c[1]*256*256)/256/256/256%256;
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







