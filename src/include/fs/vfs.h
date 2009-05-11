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

#ifndef _VFS_H
#define _VFS_H

#include <kernel/common.h>
#include <algo/btree.h>

/* vnode->flags */
#define V_FILE    0x01
#define V_DIR     0x02
#define V_ISFILE(flags) ((flags) & V_FILE)
#define V_ISDIR(flags) ((flags) & V_DIR)

/* vfs->flags */
#define VFS_READ      0x01
#define VFS_WRITE     0x02

#define MAX_FSTYPES 32
#define FSTYPE_ROOTFS 0
#define FSTYPE_INITRD 1
#define FSTYPE_DEVFS 2

struct vfs;
struct vfs_ops;
struct vnode;

typedef struct 
{
    struct vnode *v_node;
    struct vnode *v_last_node;
    u32 d_offset;
} DIR;

typedef struct 
{
    struct vnode *v_node;
    u32 f_offset;
    u32 f_flags;
} FILE;

struct vfs
{
    struct vfs_ops *vfs_ops;
    struct vnode *v_root;
    u32 vfs_flags;
    u32 vfs_dev;
    void *vfs_pdata;
    struct btree *vfs_mounts;
};

struct vnode
{
    char v_name[256];
    struct vfs *v_vfs;
    u8 v_flags;
    u32 v_inode;
    u32 v_count;
    u32 v_size;
};

struct vfs_ops
{
    /* VFS */
    int (*vfs_mount)(struct vfs*);
    int (*vfs_unmount)(struct vfs*);

    /* VNODE */
    size_t (*v_read)(FILE*, void*, size_t);
    size_t (*v_write)(FILE*, const void*, size_t);
    struct vnode *(*v_readdir)(struct vnode*, u32);
};

struct fs_type
{
    char name[32];
    struct vfs_ops *vfs_ops;
};

void vfs_init();
void register_fstype(int, struct fs_type*);

int vfs_mount(struct vnode*, int, u32, u32);
int vfs_unmount(struct vnode*);
struct vnode *vfs_lookup(struct vnode*, char*);
size_t vfs_read(FILE*, void*, size_t);
size_t vfs_write(FILE*, const void*, size_t);
void vfs_close(FILE*);
FILE *vfs_open(struct vnode*, u8);
DIR *vfs_opendir(struct vnode*);
struct vnode *vfs_readdir(DIR*);
void vfs_closedir(DIR*);

struct vnode *create_empty_vnode(struct vfs*);
struct vfs *create_empty_vfs(int fsnum);
inline void debug_vnode(struct vnode*);

inline struct vnode *get_real_node(struct vnode*);

extern struct vnode *v_root;
int btree_cmp_u32(btree_keytype, btree_keytype);

#endif
