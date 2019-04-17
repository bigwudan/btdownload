#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include "file_metafile.h"



extern int message_track(announce *announce_node,unsigned char *buff, unsigned char *hash, unsigned char *peer_id, int port, long uploaded, long downloaded,
        long life, int numwant);

#endif
