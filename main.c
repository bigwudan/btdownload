#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_metafile.h"
#include "btmap.h"
#include "message.h"


int main(){

    int i = file_metafile_init("Divx_Avi_Asf_Wmv_Wma_Rm_Rmvb_Video_Fixer_3.23.torrent");
    int bt = 10;
    btmap_init();
    btmap_set(bt);
    btmap_set(1);
    
    char buf[1400] = {0};
    char peer_id[20] = {0};

    message_track(announce_head, buf, info_hash, peer_id, 888, 0,0,0,1   );


}

