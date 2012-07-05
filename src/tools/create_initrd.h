#ifndef _CREATE_INITRD_H
#define _CREATE_INITRD_H

#define TYPE_FILE 0x1
#define TYPE_DIR  0x2

#define NAME_LEN 12

#include <stdint.h>

typedef struct initrd_node
{
     uint32_t inode;
     uint8_t type;
     uint32_t size;
     uint32_t data;
     char name[NAME_LEN];
} __attribute__((__packed__)) initrd_node_t;
#endif
