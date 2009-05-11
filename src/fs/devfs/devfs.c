#include <fs/devfs/devfs.h>
#include <fs/vfs.h>
#include <mm/mem.h>
#include <lib/stdio.h>
#include <lib/string.h>

struct devfs_mountpoint *mp = 0;

struct vfs_ops devfs_vfs_ops = {
    &devfs_mount, &devfs_unmount,
    &devfs_read, &devfs_write,
    &devfs_readdir
};

void init_devfs()
{
    struct fs_type devfs_fs_type = {"devfs", &devfs_vfs_ops};
    register_fstype(FSTYPE_DEVFS, &devfs_fs_type);
}

int devfs_mount(struct vfs *vfs)
{
    ASSERT(!mp);
    mp = (struct devfs_mountpoint*) kmalloc(sizeof(struct devfs_mountpoint));
    memset(mp, 0, sizeof(struct devfs_mountpoint));

    vfs->vfs_pdata = mp;
    vfs->v_root->v_inode = 0;

    return 0;
}

int devfs_unmount(struct vfs *vfs)
{
    mp = (struct devfs_mountpoint*) vfs->vfs_pdata;
    kfree(mp);

    return 0;
}

size_t devfs_read(FILE *file, void *buf, size_t size)
{
    mp = (struct devfs_mountpoint*) file->v_node->v_vfs->vfs_pdata;

    file = NULL; buf++; return size;
}

size_t devfs_write(FILE *file, const void *buf, size_t size)
{
    mp = (struct devfs_mountpoint*) file->v_node->v_vfs->vfs_pdata;

    file = NULL; buf++; return size;
}

struct vnode *devfs_readdir(struct vnode *node, u32 offset)
{
    if (node->v_inode != 0)
        PANIC("readdir on non '/dev' in devfs is not be possible");

    struct devfs_mountpoint *mp = (struct devfs_mountpoint*) node->v_vfs->vfs_pdata;
    if (offset >= mp->devfs_num_nodes)
        return NULL;

    struct devfs_node *subject = mp->devfs_nodes;
    struct vnode *ret = create_empty_vnode(node->v_vfs);

    strcpy(ret->v_name, subject->name);
    ret->v_inode = subject->inode;
    ret->v_flags = subject->type;
    ret->v_size = 0;

    return ret;
}
