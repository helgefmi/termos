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

#include <fs/vfs.h>
#include <lib/stdio.h>
#include <lib/string.h>

#define ASSERT_FS_SUPPORT(node) \
    if ((node)->fs_type == 0) \
    { \
        PANIC("FS not supported"); \
    }

#define ASSERT_FS_ISDIR(node) \
    if (((node)->flags & FS_DIRECTORY) == 0) \
    { \
        PANIC("finddir() on a nondirectory"); \
    }

fs_node_t *fs_root = 0;
fs_node_t *current_dir = 0;
fs_type_t *fs_types[MAX_FS_TYPES] = {0};

void register_fs(u32 typenum, fs_type_t *type)
{
    memcpy((void*)fs_types[typenum], (void*)type, sizeof(fs_type_t));
}

fs_node_t *vfs_path_lookup(char *name, fs_node_t *dir)
{
    if (dir == 0)
        dir = current_dir;
}

u32 read_fs(fs_node_t *node, u32 offset, u32 size, u8 *buffer)
{
    ASSERT_FS_SUPPORT(node);

    if (node->fs_type->read)
        return node->fs_type->read(node, offset, size, buffer);

    PANIC("node->read is null");
    return 0;
}

u32 write_fs(fs_node_t *node, u32 offset, u32 size, u8 *buffer)
{
    ASSERT_FS_SUPPORT(node);

    if (node->fs_type->write)
        return node->fs_type->write(node, offset, size, buffer);

    PANIC("node->write is null");
    return 0;
}

void open_fs(fs_node_t *node, u8 read, u8 write)
{
    ASSERT_FS_SUPPORT(node);

    if (node->fs_type->open)
        return node->fs_type->open(node);

    PANIC("node->open is null");
}

void close_fs(fs_node_t *node)
{
    ASSERT_FS_SUPPORT(node);

    if (node->fs_type->close)
        return node->fs_type->close(node);

    PANIC("node->close is null");
}

struct dirent *readdir_fs(fs_node_t *node, u32 idx)
{
    ASSERT_FS_SUPPORT(node);
    ASSERT_FS_ISDIR(node);

    if (node->fs_type->readdir)
        return node->fs_type->readdir(node, idx);

    PANIC("node->readdir is null");
    return 0;
}

fs_node_t *finddir_fs(fs_node_t *node, char *name)
{
    ASSERT_FS_SUPPORT(node);
    ASSERT_FS_ISDIR(node);

    if (node->fs_type->finddir)
        return node->fs_type->finddir(node, name);

    PANIC("node->finddir is null");
    return 0;
}

fs_type_t *get_fstype(u32 num)
{
    if (num < MAX_FS_TYPES)
        if (fs_types[num] != 0)
            return fs_types[num];

    return -1;
}
