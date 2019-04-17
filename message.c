#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "message.h"
#include "file_metafile.h"


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




