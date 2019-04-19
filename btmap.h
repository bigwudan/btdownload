#ifndef _BTMAP_H_
#define _BTMAP_H_

int btmap_init();
int btmap_set(int val);
int btmap_get_val(int val);
extern unsigned char *btmap_list ;
extern unsigned int bitmap_len ;
extern unsigned int bitmap_valid_len ;
#endif
