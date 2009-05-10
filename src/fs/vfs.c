/* 
 * This file is part of TermOS.
 *
 * TermOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TermOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TermOS.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <lib/stdio.h>
#include <lib/string.h>
#include <mm/mem.h>
#include <fs/vfs.h>
#include <fs/vfs_cache.h>
#include <algo/btree.h>

struct vnode *v_root = 0;
struct fs_type fstypes[MAX_FSTYPES] = {{"",0},};

void vfs_init()
{
    v_root = create_empty_vnode(NULL);
    v_root->v_name[0] = '/';
    v_root->v_flags = V_DIR;

    vfs_cache_init();
    btree_insert(v_cache, "/", v_root);
}

FILE *vfs_open(struct vnode *node, u8 flags)
{
    vget(node);

    FILE *ret = (FILE*) kmalloc(sizeof(FILE));
    ret->v_node = node;
    ret->f_offset = 0;
    ret->f_flags = flags;

    return ret;
}

DIR *vfs_opendir(struct vnode *node)
{
    vget(node);

    DIR *ret =(DIR*) kmalloc(sizeof(DIR));
    ret->v_node = node;
    ret->d_offset = 0;

    return ret;
}

size_t vfs_read(FILE *file, void *buf, size_t len)
{
    struct vnode *node = GET_REAL_NODE(file->v_node);
    struct vfs_ops *vfs_ops = node->v_vfs->vfs_ops;

    return vfs_ops->v_read(file, buf, len);
}

struct vnode *vfs_readdir(DIR *dir)
{
    struct vnode *node = GET_REAL_NODE(dir->v_node);
    struct vfs_ops *vfs_ops = node->v_vfs->vfs_ops;

    return vfs_ops->v_readdir(node, dir->d_offset++);
}

void vfs_close(FILE *file)
{
    vput(file->v_node);
    kfree(file);
}

void vfs_closedir(DIR *dir)
{
    vput(dir->v_node);
    kfree(dir);
}

size_t vfs_write(FILE *file, const void *buf, size_t len)
{
    struct vnode *node = GET_REAL_NODE(file->v_node);
    struct vfs_ops *vfs_ops = node->v_vfs->vfs_ops;

    return vfs_ops->v_write(file, buf, len);
}

struct vnode *vfs_lookup(struct vnode *parent, char *name)
{
    parent = GET_REAL_NODE(parent);

    struct vfs_ops *vfs_ops = parent->v_vfs->vfs_ops;
    return vfs_ops->vfs_lookup(parent, name);
}

int vfs_mount(struct vnode *node, int fsnum, u32 dev, u32 flags)
{
    if (!V_ISDIR(node->v_flags))
        PANIC("Mountpoint is not a directory");
    if (node->mount_vfs)
        PANIC("Mountpoint is already mounted");

    ASSERT(fstypes[fsnum].vfs_ops);

    struct vfs *new_vfs = (struct vfs*) kmalloc(sizeof(struct vfs));
    struct vnode *new_root = create_empty_vnode(NULL);

    new_vfs->vfs_ops = fstypes[fsnum].vfs_ops;
    new_vfs->vfs_flags = flags;
    new_vfs->vfs_dev = dev;

    new_root->v_name[0] = '/';
    new_root->v_flags = V_DIR;

    node->mount_vfs = new_vfs;
    new_root->v_vfs = new_vfs;
    new_vfs->v_root = new_root;

    int ret = fstypes[fsnum].vfs_ops->vfs_mount(new_vfs);

    if (!ret)
        vget(node); 

    return ret;
}

struct vnode *create_empty_vnode(struct vfs *vfs)
{
    struct vnode *ret = (struct vnode*) kmalloc(sizeof(struct vnode));
    memset(ret, 0, sizeof(struct vnode));

    if (vfs)
        ret->v_vfs = vfs;

    return ret;
}

void register_fstype(int fsnum, struct fs_type *new_fs)
{
    ASSERT(fsnum < MAX_FSTYPES && !fstypes[fsnum].vfs_ops);
    memcpy(&fstypes[fsnum], new_fs, sizeof(struct fs_type));
}

inline void debug_vnode(struct vnode *node)
{
    printf("<vnode 0x%x: '%s', vfs:%x, m_vfs:%x, f:%x, i:%x>\n",
        node, node->v_name, node->v_vfs,
        node->mount_vfs, node->v_flags, node->v_inode);
}
