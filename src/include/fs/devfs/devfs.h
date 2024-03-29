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
