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

/* vnode->flags */
#define V_FILE    0x01
#define V_DIR     0x02
#define V_ISFILE(flags) ((flags) & V_FILE)
#define V_ISDIR(flags) ((flags) & V_DIR)

/* vfs->flags */
#define VFS_READ      0x01

#define MAX_FSTYPES 32
#define FSTYPE_INITRD 0

#define GET_REAL_NODE(node) ((node)->mount_vfs ? (node)->mount_vfs->v_root : node)

struct vfs_ops;
struct vnode;
struct dirent;
struct FILE;

typedef struct 
{
    struct vnode *v_node;
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
};

struct vnode
{
    char v_name[256];
    struct vfs *v_vfs;
    struct vfs *mount_vfs;
    u8 v_flags;
    u32 v_inode;
    u32 v_count;
    u32 v_size;
};

struct vfs_ops
{
    /* VFS */
    int (*vfs_mount)(struct vfs*);
    struct vnode *(*vfs_lookup)(struct vnode*, char*);

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
struct vnode *vfs_lookup(struct vnode*, char*);
size_t vfs_read(FILE*, void*, size_t);
size_t vfs_write(FILE*, const void*, size_t);
void vfs_close(FILE*);
FILE *vfs_open(struct vnode*, u8);
DIR *vfs_opendir(struct vnode*);
struct vnode *vfs_readdir(DIR*);
void vfs_closedir(DIR*);

struct vnode *create_empty_vnode();
inline void debug_vnode(struct vnode*);

extern struct vnode *v_root;

#endif
