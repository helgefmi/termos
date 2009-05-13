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
#include <fs/rootfs.h>
#include <fs/vfs_cache.h>
#include <algo/btree.h>

struct vnode *v_root = 0;
struct fs_type fstypes[MAX_FSTYPES] = {{"",0},};

void vfs_init()
{
    init_rootfs();

    v_root = create_empty_vnode(NULL);
    v_root->v_name[0] = '/';
    v_root->v_flags = V_DIR;

    v_root->v_vfs = create_empty_vfs(FSTYPE_ROOTFS);
    v_root->v_vfs->v_root = v_root;

    vget(v_root);

    //vfs_cache_init();
    //btree_insert(v_cache, "/", v_root);
}

FILE *vfs_open(struct vnode *node, u8 flags)
{
    ASSERT(V_ISFILE(node->v_flags));

    vget(node);

    FILE *ret = (FILE*) kmalloc(sizeof(FILE));
    ret->v_node = node;
    ret->f_offset = 0;
    ret->f_flags = flags;

    return ret;
}

void vfs_close(FILE *file)
{
    vput(file->v_node);
    kfree(file);
}

DIR *vfs_opendir(struct vnode *node)
{
    node = get_real_node(node);
    vget(node);

    DIR *ret = (DIR*) kmalloc(sizeof(DIR));
    ret->v_node = node;
    ret->v_last_node = 0;
    ret->d_offset = 0;

    return ret;
}

void vfs_closedir(DIR *dir)
{
    vput(dir->v_node);
    kfree(dir);
}

size_t vfs_read(FILE *file, void *buf, size_t len)
{
    struct vfs_ops *vfs_ops = file->v_node->v_vfs->vfs_ops;

    return vfs_ops->v_read(file, buf, len);
}

struct vnode *vfs_readdir(DIR *dir)
{
    if (dir->v_last_node)
        vput(dir->v_last_node);

    struct vfs_ops *vfs_ops = dir->v_node->v_vfs->vfs_ops;

    struct vnode *ret = vfs_ops->v_readdir(dir->v_node, dir->d_offset++);
    dir->v_last_node = ret;

    if (ret)
        vget(ret);

    return ret;
}

size_t vfs_write(FILE *file, const void *buf, size_t len)
{
    struct vfs_ops *vfs_ops = file->v_node->v_vfs->vfs_ops;

    return vfs_ops->v_write(file, buf, len);
}

struct vnode *vfs_lookup(struct vnode *parent, char *name)
{
    parent = get_real_node(parent);

    DIR *dh = vfs_opendir(parent);
    struct vnode *node = 0;

    while ((node = vfs_readdir(dh)) != NULL)
    {
        if (!strcmp(node->v_name, name))
        {
            vfs_closedir(dh);
            return node;
        }
    }

    vfs_closedir(dh);
    return NULL;
}

int vfs_unmount(struct vnode *node)
{
    ASSERT(V_ISDIR(node->v_flags));

    struct vnode *m_node = get_real_node(node);
    ASSERT(m_node != node);
    ASSERT(m_node->v_count == 1);

    int ret = m_node->v_vfs->vfs_ops->vfs_unmount(m_node->v_vfs);
    kfree(m_node->v_vfs);

    if (!ret)
    {
        btree_delete(node->v_vfs->vfs_mounts, (void*) node->v_inode);
        vput(m_node);
    }

    return ret;
}

int vfs_mount(struct vnode *node, int fsnum, u32 dev, u32 flags)
{
    ASSERT(V_ISDIR(node->v_flags));
    ASSERT(fstypes[fsnum].vfs_ops);
    ASSERT(node->v_count == 1);

    struct vfs *new_vfs = create_empty_vfs(fsnum);
    struct vnode *new_root = create_empty_vnode(new_vfs);

    new_vfs->vfs_flags = flags;
    new_vfs->vfs_dev = dev;
    new_vfs->v_root = new_root;

    new_root->v_name[0] = '/';
    new_root->v_flags = V_DIR;

    int ret = fstypes[fsnum].vfs_ops->vfs_mount(new_vfs);

    if (!ret)
    {
        btree_insert(node->v_vfs->vfs_mounts, (void*) node->v_inode, new_vfs);
        vget(new_root);
    }

    return ret;
}

inline struct vnode *get_real_node(struct vnode *node)
{
    struct vfs *vfs_mounted = btree_lookup(node->v_vfs->vfs_mounts, (void*) node->v_inode);
    if (vfs_mounted)
        return vfs_mounted->v_root;

    return node;
}

struct vfs *create_empty_vfs(int fsnum)
{
    struct vfs *ret = (struct vfs*) kmalloc(sizeof(struct vfs));
    memset(ret, 0, sizeof(struct vfs));
    ret->vfs_ops = fstypes[fsnum].vfs_ops;
    ret->vfs_mounts = btree_create(&btree_cmp_u32);

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
    struct vnode *real_node = get_real_node(node);
    printf("<vnode 0x%x(%x): '%s', vfs:%x, f:%x, i:%x c:%d>\n",
        node, real_node, node->v_name, node->v_vfs,
        node->v_flags, node->v_inode,
        node->v_count);
}

int btree_cmp_u32(btree_keytype a, btree_keytype b)
{
    u32 inode_a = (u32) a,
        inode_b = (u32) b;

    return inode_a - inode_b;
}
