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

#include <lib/stdio.h>
#include <lib/string.h>
#include <fs/initrd.h>
#include <fs/vfs.h>
#include <mm/mem.h>

struct vfs_ops initrd_vfs_ops = {
    &initrd_mount, &initrd_unmount,
    &initrd_read, 0,
    &initrd_readdir
};

void init_initrd()
{
    struct fs_type initrd_fs_type = {"initrd", &initrd_vfs_ops};
    register_fstype(FSTYPE_INITRD, &initrd_fs_type);
}

struct vnode *initrd_readdir(struct vnode *node, u32 offset)
{
    struct initrd_mountpoint *mp = (struct initrd_mountpoint*) node->v_vfs->vfs_pdata;
    struct initrd_node *initrd_node = &mp->initrd_nodes[node->v_inode];

    ASSERT(initrd_node->type & TYPE_DIR);

    if (offset >= initrd_node->size)
        return NULL;

    u32 *data = (u32*) (mp->data_start + initrd_node->data);

    struct initrd_node *subject = &mp->initrd_nodes[data[ offset ] - 1];

    struct vnode *ret = create_empty_vnode(node->v_vfs);
    strcpy(ret->v_name, subject->name);
    ret->v_inode = subject->inode - 1;
    ret->v_flags = subject->type;
    ret->v_size = subject->size;
    return ret;
}

size_t initrd_read(FILE *file, void *buf, size_t len)
{
    struct vnode *node = file->v_node;
    struct initrd_mountpoint *mp = (struct initrd_mountpoint*) node->v_vfs->vfs_pdata;
    struct initrd_node *initrd_node = &mp->initrd_nodes[node->v_inode];

    if ((initrd_node->type & TYPE_FILE) == 0)
        return 0;

    if (file->f_offset + len > node->v_size)
        len = node->v_size - file->f_offset;

    size_t ret = len;
    const char *src = (const char *)  mp->data_start + initrd_node->data;
    char *dst = (char *) buf;
    while (len)
    {
        *dst++ = *src++;
        --len;
        file->f_offset += 1;
    }

    return ret;
}

int initrd_unmount(struct vfs *vfs)
{
    struct initrd_mountpoint *mp = (struct initrd_mountpoint*) vfs->vfs_pdata;
    kfree(mp);

    return 0;
}

int initrd_mount(struct vfs *vfs)
{
    struct initrd_mountpoint *mp = (struct initrd_mountpoint*) kmalloc(sizeof(struct initrd_mountpoint));

    u32 start_addr = vfs->vfs_dev;

    mp->initrd_headers = (struct initrd_header*) start_addr;
    mp->initrd_nodes = (struct initrd_node*) (start_addr + sizeof(struct initrd_header));
    mp->data_start = (u32)mp->initrd_nodes + (sizeof(struct initrd_node) * mp->initrd_headers->nodes);

    vfs->vfs_pdata = mp;
    vfs->v_root->v_inode = 0;

    return 0;
}
