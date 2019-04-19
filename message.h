#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include "file_metafile.h"



extern int message_track(announce *announce_node,unsigned char *buff, unsigned char *hash, unsigned char *peer_id, int port, long uploaded, long downloaded,
        long life, int numwant);


extern int create_handshake_msg(char *buff, unsigned char *hash, char *peer_id);
extern int create_keep_alive_msg(char *buff);
extern int create_keep_alive_msg(char *buff);
extern int create_have_msg(int index, char *buff);
extern int create_have_msg(int index, char *buff);
extern int int_to_char(int index, unsigned char *buff);
extern int char_to_int(unsigned int *index, const unsigned char *buff);
#endif
