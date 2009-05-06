#ifndef _CREATE_INITRD_H
#define _CREATE_INITRD_H

#define TYPE_FILE 0x1
#define TYPE_DIR  0x2

#define NAME_LEN 12

typedef unsigned long long int  u64;
typedef signed long long int    s64;
typedef unsigned int            u32;
typedef signed long int         s32;
typedef unsigned short          u16;
typedef signed short            s16;
typedef unsigned char           u8;
typedef signed char             s8;

typedef struct initrd_node
{
     u32 inode;
     u8 type;
     u32 size;
     u64 data;
     u8 name[NAME_LEN];
} __attribute__((__packed__)) initrd_node_t;
#endif
