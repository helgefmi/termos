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

typedef struct initrd_header
{
    u32 size;
    u32 nodes;
} initrd_header_t;

typedef struct initrd_node
{
     u32 inode;
     u8 type;
     u32 size;
     u32 data;
     u8 name[256];
} __attribute__((__packed__)) initrd_node_t;

void init_initrd(u32);

u32 initrd_read(fs_node_t*, u32, u32, u8*);
void initrd_open(fs_node_t*);
void initrd_close(fs_node_t*);
struct dirent *initrd_readdir(fs_node_t*, u32);
fs_node_t *initrd_finddir(fs_node_t*, char*);

#endif
