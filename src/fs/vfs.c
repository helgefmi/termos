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
#include <mm/mem.h>
#include <fs/vfs.h>

struct vnode *v_root;
struct fs_type fstypes[MAX_FSTYPES] = {{"",0,0},};

void vfs_init()
{
    v_root = create_empty_vnode();
    v_root->v_name[0] = '/';
    v_root->v_flags = V_DIR;
}

void vfs_mount(struct vnode *node, int fsnum, u32 dev, u32 flags)
{
    if (!V_ISDIR(node->v_flags))
    {
        PANIC("Mountpoint is not a directory");
    }
    if (node->mount_vfs)
    {
        PANIC("Mountpoint is already mounted");
    }

    struct vfs *new_vfs = (struct vfs*)kmalloc(sizeof(struct vfs));
    struct vnode *new_root = create_empty_vnode();

    node->mount_vfs = new_vfs;

    new_vfs->vfs_ops = fstypes[fsnum].vfs_ops;
    new_vfs->v_ops = fstypes[fsnum].v_ops;
    new_vfs->v_root = new_root;
    new_vfs->vfs_flags = flags;
    new_vfs->vfs_dev = dev;

    new_root->v_name[0] = '/';
    new_root->v_vfs = new_vfs;
    new_root->v_flags = V_DIR;
    new_root->next = node->next;
    new_root->prev = node->prev;

    if (fstypes[fsnum].vfs_ops->vfs_mount(new_vfs))
    {
        PANIC("mount returned an error");
    }
}

struct vnode *create_empty_vnode()
{
    struct vnode *ret = (struct vnode*) kmalloc(sizeof(struct vnode));
    memset(ret, 0, sizeof(struct vnode));
    return ret;
}

void register_fstype(int fsnum, struct fs_type *new_fs)
{
    ASSERT(fsnum < MAX_FSTYPES && !fstypes[fsnum].vfs_ops);

    memcpy(&fstypes[fsnum], new_fs, sizeof(struct fs_type));
}
