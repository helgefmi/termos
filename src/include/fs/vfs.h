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

#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_CHARDEVICE  0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE        0x05
#define FS_SYMLINK     0x06
#define FS_MOUNTPOINT  0x08

#define MAX_FS_TYPES   32

#include <kernel/common.h>

typedef struct dirent
{
    char name[128];
    u32 ino; 
} dirent_t;

struct fs_node;

typedef u32 (*fs_read_t)(struct fs_node*, u32, u32, u8*);
typedef u32 (*fs_write_t)(struct fs_node*, u32, u32, u8*);
typedef void (*fs_open_t)(struct fs_node*);
typedef void (*fs_close_t)(struct fs_node*);
typedef struct dirent *(*fs_readdir_t)(struct fs_node*, u32);
typedef struct fs_node *(*fs_finddir_t)(struct fs_node*, char*);

typedef struct fs_type
{
    fs_read_t read;
    fs_write_t write;
    fs_open_t open;
    fs_close_t close;
    fs_readdir_t readdir;
    fs_finddir_t finddir;
} fs_type_t;

typedef struct fs_node
{
    char name[128];
    u32 mask;
    u32 uid, gid;
    u16 flags;         /* Includes the node type. See #defines above. */
    u32 inode;         /* This is device-specific - provides a way for a filesystem to identify files. */
    u32 size;          /* Size of the file, in bytes. */
    u32 impl;          /* An implementation-defined number. */
    struct fs_node *ptr;
    struct fs_type *fs_type;
} fs_node_t; 

extern fs_node_t *fs_root;

u32 read_fs(fs_node_t*, u32, u32, u8*);
u32 write_fs(fs_node_t*, u32, u32, u8*);
void open_fs(fs_node_t*, u8, u8);
void close_fs(fs_node_t*);
struct dirent *readdir_fs(fs_node_t*, u32);
fs_node_t *finddir_fs(fs_node_t*, char*); 

void register_fs(u32 type, fs_type_t*);
fs_type_t *get_fstype(u32);
fs_node_t *vfs_path_lookup(char*, fs_node_t*);

#endif
