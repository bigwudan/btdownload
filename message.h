#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include "file_metafile.h"
#include "peer.h"

extern int message_track(announce *announce_node,unsigned char *buff, unsigned char *hash, unsigned char *peer_id, int port, long uploaded, long downloaded,
        long life, int numwant);


extern int create_handshake_msg(char *buff, unsigned char *hash, char *peer_id);
extern int create_keep_alive_msg(char *buff);
extern int create_have_msg(int index, char *buff);
extern int int_to_char(int index, unsigned char *buff);
extern int char_to_int(unsigned int *index, const unsigned char *buff);


extern int create_bitfield_msg(char *bitfield,int bitfield_len, char *buff);

extern int create_request_msg(int index,int begin,int length, char *buff);

extern int create_piece_msg(int index,int begin,char *block,int b_len,char *buff);
int create_cancel_msg(int index,int begin,int length,char *buff);


int is_complete_message(unsigned char *buff,unsigned int len,int *ok_len);

int is_complete_message(unsigned char *buff,unsigned int len,int *ok_len);

void discard_send_buffer(Peer *peer);
int process_handshake_msg(Peer *peer,unsigned char *buff,int len);

int process_choke_msg(Peer *peer,unsigned char *buff,int len);
#endif
