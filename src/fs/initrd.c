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

struct vfs_ops vfs_ops = {&initrd_mount, &initrd_lookup};
struct v_ops v_ops = {};

void init_initrd()
{
    struct fs_type initrd_fs_type = {"initrd", &vfs_ops, &v_ops};
    register_fstype(FSTYPE_INITRD, &initrd_fs_type);
}

struct vnode *initrd_lookup(struct vnode *parent, char *name)
{
    struct initrd_mountpoint *mp = (struct initrd_mountpoint*) parent->v_vfs->vfs_pdata;
    struct initrd_node *parent_node = &mp->initrd_nodes[parent->v_inode];

    u32 i;

    ASSERT(parent_node->type & TYPE_DIR);

    u32 *data = (u32*) (mp->data_start + parent_node->data);

    for (i = 0; i < parent_node->size; ++i)
    {
        struct initrd_node *subject = &mp->initrd_nodes[data[i] - 1];

        if (strcmp(subject->name, name) == 0)
        {
            struct vnode *ret = create_empty_vnode(parent->v_vfs);
            strcpy(ret->v_name, subject->name);
            ret->v_inode = subject->inode - 1;
            ret->v_flags = subject->type;
            return ret;
        }
    }

    return NULL;
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
