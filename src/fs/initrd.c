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

#include <fs/initrd.h>
#include <lib/stdio.h>
#include <mm/mem.h>
#include <lib/string.h>

static initrd_header_t *initrd_header;
static initrd_node_t *initrd_nodes;

fs_type_t initrd_fs = {
    (fs_read_t)initrd_read,
    (fs_write_t)0,
    (fs_open_t)initrd_open,
    (fs_close_t)initrd_close,
    (fs_readdir_t)initrd_readdir,
    (fs_finddir_t)initrd_finddir
};
void init_initrd(u32 start_addr)
{
    register_fs(0, &initrd_fs);

    initrd_header = (initrd_header_t*) start_addr;
    printf("%d\n%d\n", initrd_header->size, initrd_header->nodes);

    initrd_nodes = (initrd_node_t*)(start_addr + sizeof(initrd_header_t));
}

u32 initrd_read(fs_node_t *node, u32 offset, u32 size, u8 *buf)
{
    if (offset > node->size)
        return 0;
    if (offset + size > node->size)
        size = node->size - offset;

    memcpy(buf, (u8*)(initrd_nodes[node->inode].data + offset), size);
    return size;
}

struct dirent *initrd_readdir(fs_node_t *node, u32 idx)
{
}

fs_node_t *initrd_finddir(fs_node_t *node, char *name)
{
}

void initrd_open(fs_node_t *node)
{
    node = 0;
}

void initrd_close(fs_node_t *node)
{
    node = 0;
}

