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

#ifndef _FS_H
#define _FS_H

#include "common.h"

#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_CHARDEVICE  0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE        0x05
#define FS_SYMLINK     0x06
#define FS_MOUNTPOINT  0x08

#include "common.h"

typedef struct dirent
{
    char name[128];
    u32 ino; 
} dirent_t;

struct fs_node;

typedef u32 (*read_type_t)(struct fs_node*, u32, u32, u8*);
typedef u32 (*write_type_t)(struct fs_node*, u32, u32, u8*);
typedef void (*open_type_t)(struct fs_node*);
typedef void (*close_type_t)(struct fs_node*);
typedef struct dirent *(*readdir_type_t)(struct fs_node*, u32);
typedef struct fs_node *(*finddir_type_t)(struct fs_node*, char *name);

typedef struct fs_type
{
    read_type_t read;
    write_type_t write;
    open_type_t open;
    close_type_t close;
    readdir_type_t readdir;
    finddir_type_t finddir;
} fs_type_t;

typedef struct fs_node
{
    char name[128];
    u32 mask;
    u32 uid, gid;
    u16 flags;         /* Includes the node type. See #defines above. */
    u32 inode;         /* This is device-specific - provides a way for a filesystem to identify files. */
    u32 length;        /* Size of the file, in bytes. */
    u32 impl;          /* An implementation-defined number. */
    struct fs_node *ptr;
    u32 fsid;
} fs_node_t; 

extern fs_node_t *fs_root;
extern fs_type_t *fs_types[32];

u32 read_fs(fs_node_t*, u32, u32, u8*);
u32 write_fs(fs_node_t*, u32, u32, u8*);
void open_fs(fs_node_t*, u8, u8);
void close_fs(fs_node_t*);
struct dirent *readdir_fs(fs_node_t*, u32);
fs_node_t *finddir_fs(fs_node_t*, char*); 

void register_fs(u32 type, fs_type_t*);

#endif
