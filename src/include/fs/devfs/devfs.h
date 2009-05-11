#ifndef _DEVFS_H
#define _DEVFS_H

#include <kernel/common.h>
#include <fs/vfs.h>

struct devfs_node
{
    char name[256];
    u32 inode;
    u8 type;
};

struct devfs_mountpoint
{
    struct devfs_node *devfs_nodes;
    u32 devfs_num_nodes;
};

void init_devfs();

int devfs_mount(struct vfs*);
int devfs_unmount(struct vfs*);
size_t devfs_read(FILE*, void*, size_t);
size_t devfs_write(FILE*, const void*, size_t);
struct vnode *devfs_readdir(struct vnode*, u32);

#endif
