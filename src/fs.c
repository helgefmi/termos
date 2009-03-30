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

#include "fs.h"
#include "stdio.h"
#include "string.h"

#define ASSERT_FS_SUPPORT(node) \
    if (fs_types[(node)->fsid] == 0) \
    { \
        PANIC("FS not supported"); \
    }

#define ASSERT_FS_ISDIR(node) \
    if (((node)->flags & FS_DIRECTORY) == 0) \
    { \
        PANIC("finddir() on a nondirectory"); \
    }

fs_node_t *fs_root = 0;
fs_type_t *fs_types[32] = {0};

void register_fs(u32 typenum, fs_type_t *type)
{
    memcpy((void*)fs_types[typenum], (void*)type, sizeof(fs_type_t));
}

u32 read_fs(fs_node_t *node, u32 offset, u32 size, u8 *buffer)
{
    ASSERT_FS_SUPPORT(node);

    if (fs_types[node->fsid]->read)
        return fs_types[node->fsid]->read(node, offset, size, buffer);

    PANIC("node->read is null");
    return 0;
}

u32 write_fs(fs_node_t *node, u32 offset, u32 size, u8 *buffer)
{
    ASSERT_FS_SUPPORT(node);

    if (fs_types[node->fsid]->write)
        return fs_types[node->fsid]->write(node, offset, size, buffer);

    PANIC("node->write is null");
    return 0;
}

void open_fs(fs_node_t *node, u8 read, u8 write)
{
    ASSERT_FS_SUPPORT(node);

    if (fs_types[node->fsid]->open)
        return fs_types[node->fsid]->open(node);

    PANIC("node->open is null");
}

void close_fs(fs_node_t *node)
{
    ASSERT_FS_SUPPORT(node);

    if (fs_types[node->fsid]->close)
        return fs_types[node->fsid]->close(node);

    PANIC("node->close is null");
}

struct dirent *readdir_fs(fs_node_t *node, u32 idx)
{
    ASSERT_FS_SUPPORT(node);
    ASSERT_FS_ISDIR(node);

    if (fs_types[node->fsid]->readdir)
        return fs_types[node->fsid]->readdir(node, idx);

    PANIC("node->readdir is null");
    return 0;
}

fs_node_t *finddir_fs(fs_node_t *node, char *name)
{
    ASSERT_FS_SUPPORT(node);
    ASSERT_FS_ISDIR(node);

    if (fs_types[node->fsid]->finddir)
        return fs_types[node->fsid]->finddir(node, name);

    PANIC("node->finddir is null");
    return 0;
}
