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
#include <fs/vfs.h>
#include <mm/mem.h>

struct vfs_ops rootfs_vfs_ops = {
    0, 0,
    0, 0,
    0
};

void init_rootfs()
{
    struct fs_type rootfs_fs_type = {"rootfs", &rootfs_vfs_ops};
    register_fstype(FSTYPE_ROOTFS, &rootfs_fs_type);
}
