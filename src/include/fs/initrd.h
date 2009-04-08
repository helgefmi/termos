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

#ifndef _INITRD_H
#define _INITRD_H

#include <kernel/common.h>
#include <fs/vfs.h>

#define TYPE_FILE 0x1
#define TYPE_DIR  0x2

struct initrd_header
{
    u32 size;
    u32 nodes;
};

struct initrd_node
{
     u32 inode;
     u8 type;
     u32 size;
     u32 data;
     u8 name[256];
} __attribute__((__packed__));

struct initrd_mountpoint
{
    struct initrd_header *initrd_headers;
    struct initrd_node *initrd_nodes;
    u32 data_start;
};

void init_initrd();
int initrd_mount(struct vfs*);

#endif
