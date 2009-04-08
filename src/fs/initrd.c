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

struct vfs_ops vfs_ops = {&initrd_mount};
struct v_ops v_ops = {0, 0};

void init_initrd()
{
    struct fs_type initrd_fs_type = {"initrd", &vfs_ops, &v_ops};
    register_fstype(FSTYPE_INITRD, &initrd_fs_type);
}

int initrd_mount(struct vfs *vfs)
{
    struct initrd_mountpoint *mp = (struct initrd_mountpoint*) kmalloc(sizeof(struct initrd_mountpoint));

    u32 start_addr = vfs->vfs_dev;

    mp->initrd_headers = (struct initrd_header*) start_addr;
    mp->initrd_nodes = (struct initrd_node*) (start_addr + sizeof(struct initrd_header));
    mp->data_start = (u32)mp->initrd_nodes + (sizeof(struct initrd_node) * mp->initrd_headers->nodes);

    vfs->vfs_pdata = mp;

    return 0;
}
