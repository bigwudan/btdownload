#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_metafile.h"
#include "btmap.h"


int main(){

    int i = file_metafile_init("Divx_Avi_Asf_Wmv_Wma_Rm_Rmvb_Video_Fixer_3.23.torrent");
    int bt = 10;
    btmap_init();
    btmap_set(bt);
    btmap_set(1);
    printf("flag=%d\n", btmap_get_val(10));

    printf("flag=%d\n", btmap_get_val(2));
}

