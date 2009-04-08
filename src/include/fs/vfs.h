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

/* defines for vnode->flags */
#define V_FILE    0x01
#define V_DIR     0x02

#define V_ISDIR(flags) ((flags) & V_DIR)

/* defines for vfs->flags */
#define VFS_READ      0x01

#define MAX_FSTYPES 32
#define FSTYPE_INITRD 0

struct vfs_ops;
struct v_ops;
struct vnode;

struct vfs
{
    struct vfs_ops *vfs_ops;
    struct v_ops *v_ops;
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
    struct vnode *next;
    struct vnode *prev;
};

struct vfs_ops
{
    int (*vfs_mount)(struct vfs*);
};

struct v_ops
{
    int (*v_read)();
    int (*v_readdir)();
};

struct fs_type
{
    char name[32];
    struct vfs_ops *vfs_ops;
    struct v_ops *v_ops;
};

void vfs_init();
void register_fstype(int, struct fs_type*);

struct vnode *create_empty_vnode();
struct vnode *v_first_sibling(struct vnode*);
struct vnode *v_parent(struct vnode*);

void vfs_mount(struct vnode*, int, u32, u32);

extern struct vnode *v_root;

#endif
